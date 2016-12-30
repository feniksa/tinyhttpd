#pragma once

#include "cpsp/NotFoundHandler.h"

class HandleNotFound : public NotFoundHandler
{
public:
	using NotFoundHandler::NotFoundHandler;

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

void notFound(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
