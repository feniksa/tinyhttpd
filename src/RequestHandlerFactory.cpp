#include "RequestHandlerFactory.h"

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>

#include <Poco/Util/Application.h>
#include <Poco/URI.h>

#include "api/DirectoryHandler.h"
#include "HandleStaticFile.h"
#include "cpsp/DirectoryHandler.h"
#include "HandleNotFound.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>

RequestHandlerFactory::RequestHandlerFactory(const WebDirectories& webDirectories)
: m_webDirectories(webDirectories)
{
}

bool RequestHandlerFactory::processUrlSufix(const std::string& prefix, const std::string& path, std::string& sufix)
{
	std::string::const_iterator prefi = prefix.begin();
	std::string::const_iterator pathi = path.begin();

	while (prefi != prefix.end() && pathi != path.end() && *prefi == *pathi) {
		++prefi;
		++pathi;
	}

	if (prefi != prefix.end())
		return false;

	sufix.clear();
	sufix.reserve(std::distance(pathi, path.end()));

	while (pathi != path.end()) {
		sufix += *pathi;
		++pathi;
	}

	return true;
}

Poco::Net::HTTPRequestHandler *RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
{
	Poco::Util::Application &app = Poco::Util::Application::instance();
	app.logger().information("Request URI " + request.getURI());

	//Poco::URI uri(request.getURI());

	/*using callback_t = std::function<Poco::Net::HTTPRequestHandler*()>;
	using callmap_t = std::map<std::string, callback_t>;*/

	//static callmap_t callmap {
		//std::make_pair("/api/dir", [&]() { return new api::DirectoryHandler(_docroot);  })
	//};

	std::string path;
	std::string sufixPath;
	bool result;
	for (WebDirectories::const_iterator i = m_webDirectories.begin(); i != m_webDirectories.end(); ++i) {

		const WebDirectory& webDirectory = *i;

		Poco::URI uri(request.getURI());
		result = processUrlSufix(webDirectory.m_alias, uri.getPath(), sufixPath);
		if (!result) continue;

		if (!webDirectory.m_alias.empty()) {
			path = webDirectory.m_path + boost::filesystem::path::preferred_separator + sufixPath;
		} else {
			path = sufixPath;
		}

		//const std::string path = request.getURI(); /*webDirectory.m_path + boost::filesystem::path::preferred_separator + request.getURI()*/;
		if (!boost::filesystem::exists(path))
			continue;

		if (boost::filesystem::is_directory(path)) {
			if (webDirectory.m_showContent)
				return new DirectoryHandler(path);
			else
				return new HandleNotFound();
		} else if (boost::filesystem::is_symlink(path)) {
			if (webDirectory.m_followSymLink) {
				return new HandleStaticFile(path);
			} else {
				return new HandleNotFound();
			}
		} else if (boost::filesystem::is_regular_file(path)) {
			return new HandleStaticFile(path);
		}
	}

	/*callmap_t::const_iterator iter = callmap.find(uri.getPath());
	if (iter != callmap.end()) {
		return iter->second();
	}*/


	return new HandleNotFound();
}
