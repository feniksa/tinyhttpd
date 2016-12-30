#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Path.h>
#include <Poco/JSON/Object.h>
#include <Poco/File.h>

namespace api
{

class DirectoryHandler : public Poco::Net::HTTPRequestHandler
{
public:
	DirectoryHandler(const Poco::Path& path);

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
	Poco::JSON::Object fileToObject(const Poco::File& file) const;

	const Poco::Path _path;
};

}
