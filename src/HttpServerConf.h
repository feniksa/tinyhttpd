#pragma once

#include <Poco/Net/HTTPServerParams.h>
#include <string>

struct HttpServerConf
{
	int m_port = 0;
	bool m_tcpKeepAlive = true;
	int m_maxKeepAliveRequiests = 0;
	int m_maxTcpQueue = 0;
	int m_threads = 0;

	void convert(Poco::Net::HTTPServerParams*) const;
	bool load(const std::string& file);

	bool validate() const;
};
