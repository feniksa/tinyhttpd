#include "HandleNotFound.h"
// impl
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

void HandleNotFound::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	response.setStatusAndReason(Poco::Net::HTTPServerResponse::HTTP_NOT_FOUND, "Not found \""+ request.getURI() + "\"");
	NotFoundHandler::handleRequest(request, response);
}

void notFound(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
	HandleNotFound handler;
	handler.handleRequest(request, response);
}
