#include "HttpServer.h"

#include "RequestHandlerFactory.h"
#include "Configuration.h"

#include <Poco/Util/Option.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <cstdlib>

using Poco::Util::Option;
using Poco::Util::HelpFormatter;
using Poco::Net::HTTPServerParams;
using Poco::Net::ServerSocket;
using Poco::Net::HTTPServer;

const std::string  DefaultConfFile = "server.xml";
const std::string DefaultWebConfPath = "vhosts.d";
const char* TESTING_ENVIRONMENT_NAME = "HTTP_SERVER_DEBUG_TESTING";

void HttpServer::Blocker::wait(HttpServer* server)
{
	assert(server);
	server->waitForTerminationRequest();
}


HttpServer::HttpServer(std::shared_ptr<Blocker> blocker) :
 m_blocker(blocker),
 m_helpRequested(false),
 m_testArgs(false)
{
}

HttpServer::~HttpServer()
{
}

void HttpServer::initialize(Application &self)
{
    loadConfiguration(); // load default configuration files, if present
    ServerApplication::initialize(self);
}

void HttpServer::uninitialize()
{
    ServerApplication::uninitialize();
}

void HttpServer::defineOptions(Poco::Util::OptionSet &options)
{
    ServerApplication::defineOptions(options);

	options.addOption(
		Option("configuration", "c", "Path to configuration directory")
		.required(false)
		.repeatable(false)
		.argument("dir")
	);

	options.addOption(
		Option("port", "p", "Port")
		.required(false)
		.repeatable(false)
		.argument("port")
	);
	options.addOption(
		Option("test", "t", "Test")
		.required(false)
		.repeatable(false)
	);

	options.addOption(
		Option("help", "h", "Display help information on command line arguments")
		.required(false)
		.repeatable(false)
		.callback(Poco::Util::OptionCallback<HttpServer>(this, &HttpServer::handleHelp))
	);
}

void HttpServer::handleOption(const std::string &name, const std::string &value)
{
    ServerApplication::handleOption(name, value);

	//Poco::Util::Application &app = Poco::Util::Application::instance();

	if (name == "help") {
		m_helpRequested = true;
	}  else if (name == "configuration") {
		m_confDir = value;
	} else if (name == "port") {
		m_configuration.m_port = std::stoi(value);
	} else if (name == "test") {
		m_testArgs = true;
	}
}

void HttpServer::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A web server that show content of directories.");
    helpFormatter.format(std::cout);
}

void HttpServer::handleHelp(const std::string &, const std::string &)
{
    m_helpRequested = true;
    displayHelp();
    stopOptionsProcessing();
}

int HttpServer::loadServerConfiguration()
{
	if (!m_confDir.empty()) {
		int port = m_configuration.m_port;
		if (!m_configuration.load(m_confDir + boost::filesystem::path::preferred_separator + DefaultConfFile)) {
			return Application::EXIT_CONFIG;
		}

		if (port != 0) {
			m_configuration.m_port = port;
		}
	}

	if (!m_configuration.validate()) {
		return Application::EXIT_CONFIG;
	}

	return Application::EXIT_OK;
}

int HttpServer::loadWebDirectoryConfiguration(const std::vector<std::string> & args)
{
	Poco::Util::Application &app = Poco::Util::Application::instance();

	if (!m_confDir.empty()) {
		const std::string dir = m_confDir + boost::filesystem::path::preferred_separator + DefaultWebConfPath;

		if (!loadWebDirectories(dir, m_webDirectories)) {
			return Application::EXIT_CONFIG;
		}
	}

	std::for_each (args.begin(), args.end(), [this, &app](const std::string& path) {
		WebDirectory dir;

		dir.m_path = path;
		dir.m_followSymLink = true;
		dir.m_showContent = true;

		if (dir.validate()) {
			m_webDirectories.push_back(dir);
		} else {
			app.logger().warning("Directory \"" + path + "\" from command line ignored, as it doesn't validated");
		}
	});

	return Application::EXIT_OK;
}

int HttpServer::port() const
{
	return m_configuration.m_port;
}

int HttpServer::main(const std::vector<std::string> & args)
{
	int status;

    if (m_helpRequested)
        return Application::EXIT_USAGE;

	status = loadServerConfiguration();
	if (status != Application::EXIT_OK)
		return status;

	status = loadWebDirectoryConfiguration(args);
	if (status != Application::EXIT_OK)
		return status;

    HTTPServerParams *params = new HTTPServerParams;
    //params->setServerName(ServerName);
    //params->setSoftwareVersion(ServerVersion);

	m_configuration.convert(params);

    // set-up a server socket
    ServerSocket svs(m_configuration.m_port);

    // set-up a HTTPServer instance
    HTTPServer srv(new RequestHandlerFactory(m_webDirectories), svs, params);

	if (m_testArgs) {
		return Application::EXIT_OK;
	}

	// start the HTTPServer
    srv.start();

	assert(m_blocker.get());
    // wait for CTRL-C or kill
    m_blocker->wait(this);
    // Stop the HTTPServer
    srv.stop();

    return Application::EXIT_OK;
}
