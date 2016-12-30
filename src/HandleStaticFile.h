#pragma once

#include <Poco/Net/HTTPRequestHandler.h>

#include <string>
#include <vector>
#include <Poco/Path.h>
#include <Poco/File.h>
#include "WebDirectory.h"

class HandleStaticFile : public Poco::Net::HTTPRequestHandler
{
public:
	using range_t = std::pair<unsigned long long, unsigned long long>;

	HandleStaticFile(std::string filePath);
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
	bool validate(const Poco::Net::HTTPServerRequest& request) const;

	bool handleFile(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const Poco::Path& path);
	bool isCached(const Poco::Net::HTTPServerRequest& request, const Poco::File& file) const;
	bool isRanged(const Poco::Net::HTTPServerRequest& request);

	const std::string m_filePath;
};
