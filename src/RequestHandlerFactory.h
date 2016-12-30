#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <functional>
#include <map>
#include "WebDirectory.h"

class RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory
{
public:
	RequestHandlerFactory(const WebDirectories& webDirectories);

	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

	static bool processUrlSufix(const std::string& prefix, const std::string& path, std::string& sufix);
private:
	const WebDirectories& m_webDirectories;
};
