#include "HandleStaticFile.h"
#include <cassert>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include <Poco/URI.h>
#include <Poco/Path.h>
#include <Poco/File.h>

#include "HandleNotFound.h"
#include "cpsp/DirectoryHandler.h"
#include "mime.h"

// sendfile
#include <Poco/Util/Application.h>

#include <Poco/DateTimeFormat.h>
#include <Poco/Timezone.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeParser.h>
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>
#include <Poco/Buffer.h>

#include "HttpHeaderParse.h"

#include "StreamCopy.h"

const unsigned long long BufferSize = 65536;

HandleStaticFile::HandleStaticFile(std::string filePath)
: m_filePath(filePath)
{
}

bool HandleStaticFile::isCached(const Poco::Net::HTTPServerRequest& request, const Poco::File& file) const
{
	if (!request.has("If-Modified-Since"))
		return false;

	int offset = 0;
	Poco::DateTime modifiedSince = Poco::DateTimeParser::parse(Poco::DateTimeFormat::HTTP_FORMAT, request.get("If-Modified-Since"), offset);

	return (modifiedSince.timestamp() == file.getLastModified());
}

bool HandleStaticFile::isRanged(const Poco::Net::HTTPServerRequest& request)
{
	return request.has("Range");
}

void HandleStaticFile::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	if (!validate(request)) {
		return notFound(request, response);
	}

	Poco::Util::Application &app = Poco::Util::Application::instance();

	Poco::File file(m_filePath);

	if (!file.exists())
		return notFound(request, response);

	assert(!file.isDirectory());
	if (file.isDirectory())
		return notFound(request, response);

	if (file.isFile()) {
		FileRange fileRange {0, file.getSize() - 1, file.getSize()};

		// set header values
		response.setChunkedTransferEncoding(false);
		response.set("Accept-Ranges", "bytes");
		response.setContentType(mime(m_filePath));
		response.set("Last-Modified", Poco::DateTimeFormatter::format(file.getLastModified(), Poco::DateTimeFormat::HTTP_FORMAT));

		if (isRanged(request)) {
			if (!parse::bytesRange(request.get("Range"), &fileRange)) {
				app.logger().warning("Can't parse range");
				response.setStatusAndReason(Poco::Net::HTTPServerResponse::HTTP_BAD_REQUEST, "Can't parse range");
				response.send();
				return;
			}

			if (!fileRange.validate()) {
				app.logger().warning("Invalid file range");
				response.setStatusAndReason(Poco::Net::HTTPServerResponse::HTTP_BAD_REQUEST, "Bad range");
				response.send();
				return;
			}

			response.set("Content-Range", to_string(fileRange));
			response.setStatus(Poco::Net::HTTPServerResponse::HTTP_PARTIAL_CONTENT);
		} else if (isCached(request, file)) {
			app.logger().warning("File not modified");
			response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_MODIFIED, "File not modified");
			response.send();
			return;
		}

		response.setContentLength64(fileRange.length());

		std::ostream& httpStream = response.send();

		if (request.getMethod() == "HEAD") {
			return;
		}

		Poco::FileInputStream fileStream(m_filePath);
		if (!fileStream.good()) {
			return notFound(request, response);
		}

		streamCopy(fileStream, fileRange.begin(), fileRange.length(), BufferSize, &httpStream);

		return;
	}

	return notFound(request, response);
}

bool HandleStaticFile::validate(const Poco::Net::HTTPServerRequest& request) const
{
	bool badPath = (request.getURI().find("..") != std::string::npos);
	if (badPath)
		return false;
	else
		return true;
}
