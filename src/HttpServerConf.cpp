#include "HttpServerConf.h"
#include <cassert>

#include <Poco/Util/Application.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/foreach.hpp>

bool HttpServerConf::validate() const
{
	Poco::Util::Application &app = Poco::Util::Application::instance();

	if (m_port <= 0) {
		app.logger().error("not specified port parameter \"port\": \"" + std::to_string(m_port) + "\"");
		return false;
	}

	return true;
}


bool HttpServerConf::load(const std::string& filename)
{
	Poco::Util::Application &app = Poco::Util::Application::instance();

	boost::property_tree::ptree tree;

	if (!boost::filesystem::exists(filename)) {
		app.logger().error("configuration file \"" + filename + "\" not found");
		return false;
	}

	if (!boost::filesystem::is_regular(filename)) {
		app.logger().error("configuration file \"" + filename + "\" is not regular file");
		return false;
	}

	// Parse the XML into the property tree.
	boost::property_tree::read_xml(filename, tree);

	m_port = tree.get<unsigned int>("server.port") ? tree.get<unsigned int>("server.port") : 0;
	m_tcpKeepAlive = tree.get<bool>("server.tcpKeepAlive");
	m_maxKeepAliveRequiests = tree.get<int>("server.maxKeepAliveRequests");
	m_maxTcpQueue = tree.get<int>("server.maxTcpQueue");
	m_threads = tree.get<int>("server.threads");

	return true;
}


void HttpServerConf::convert(Poco::Net::HTTPServerParams* params) const
{
	assert(params);

	if (m_tcpKeepAlive) {
		params->setKeepAlive(m_tcpKeepAlive);
	}
	if (m_maxKeepAliveRequiests) {
		params->setMaxKeepAliveRequests(m_maxKeepAliveRequiests);
	}
	if (m_maxTcpQueue) {
		params->setMaxQueued(m_maxTcpQueue);
	}

	if (m_threads) {
		params->setMaxThreads(m_threads);
	}
}

