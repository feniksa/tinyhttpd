#pragma once

#include <Poco/Util/ServerApplication.h>
#include "HttpServerConf.h"
#include "WebDirectory.h"
#include <memory>

class HttpServer: public Poco::Util::ServerApplication
{
public:
	struct Blocker
	{
		virtual void wait(HttpServer* server);
	};

	explicit HttpServer(std::shared_ptr<Blocker> blocker = std::shared_ptr<Blocker>(new Blocker));
	~HttpServer();

	const HttpServerConf& getConfiguration() const { return m_configuration; }
	int port() const;

protected:
	void initialize(Application& self);
	void uninitialize();

	void defineOptions(Poco::Util::OptionSet& options);
	void handleOption(const std::string& name, const std::string& value);

	void displayHelp();
	void handleHelp(const std::string&, const std::string& value);

	int main(const std::vector<std::string>& args);

private:
	int loadServerConfiguration();
	int loadWebDirectoryConfiguration(const std::vector<std::string> & args);

	std::shared_ptr<Blocker> m_blocker;
	bool m_helpRequested;
	HttpServerConf m_configuration;
	std::string m_confDir;
	WebDirectories m_webDirectories;
	bool m_testArgs;
};
