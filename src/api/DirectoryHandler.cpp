#include "DirectoryHandler.h"

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/Application.h>
#include <Poco/File.h>
#include <Poco/SortedDirectoryIterator.h>
#include <Poco/URI.h>

namespace api
{

inline std::string fileToType(const Poco::File& file)
{
	if (file.isDirectory()) {
		return "d";
	} else if (file.isFile()) {
		return "f";
	} else if (file.isDevice()) {
		return "b";
	} else if (file.isLink()) {
		return "l";
	}

	return "";
}

inline Poco::File::FileSize fileSize(const Poco::File& file)
{
	if (file.isFile())
		return file.getSize();

	return 0;
}

DirectoryHandler::DirectoryHandler(const Poco::Path& path)
: _path(path)
{
}

Poco::JSON::Object DirectoryHandler::fileToObject(const Poco::File& file) const
{
	Poco::JSON::Object obj;

	Poco::Path path (file.path());

	Poco::URI uri;
	uri.setPath(path.getFileName());

	obj.set("type", fileToType(file));
	obj.set("size", fileSize(file));
	obj.set("name", path.getFileName());
	obj.set("name_encoded", uri.toString());
	obj.set("canRead", file.canRead());
	obj.set("modified", file.getLastModified());

	return obj;
}

void DirectoryHandler::handleRequest(Poco::Net::HTTPServerRequest&, Poco::Net::HTTPServerResponse &response)
{
	response.setChunkedTransferEncoding(true);
	response.setContentType("application/json");

	Poco::File file(_path);

	if (!file.exists()) {
		std::ostream &ostr = response.send();

		Poco::JSON::Object error;

		error.set("code", 1);
		error.set("reason", "Directory not found");

		error.stringify(ostr);

		return;
	}

	std::ostream &ostr = response.send();

	Poco::SortedDirectoryIterator dirIter(_path);
	Poco::SortedDirectoryIterator dirIterEnd;

	Poco::JSON::Array list;

	while (dirIter != dirIterEnd) {
		Poco::JSON::Object node = fileToObject(*dirIter);
		list.add(node);
		++dirIter;
	}

	list.stringify(ostr);
	ostr << "\n";
}

}
