#include <gtest/gtest.h>
#include "HttpServerConf.h"
#include "HttpServer.h"
#include "RequestHandlerFactory.h"
#include "WebDirectory.h"
#include <boost/filesystem.hpp>

struct TestHttpServer : public ::testing::Test
{
	HttpServer app;
};

TEST_F(TestHttpServer, loadServerConfiguration)
{
	HttpServerConf conf;

	bool result = conf.load("server.xml");

	EXPECT_TRUE(result);
	EXPECT_EQ(8082, conf.m_port);
	EXPECT_TRUE(conf.m_tcpKeepAlive);
	EXPECT_EQ(0, conf.m_maxKeepAliveRequiests);
	EXPECT_EQ(0, conf.m_maxTcpQueue);
	EXPECT_EQ(0, conf.m_threads);
}

TEST_F(TestHttpServer, loadServerConfiguration_NoFile)
{
	HttpServerConf server;

	bool result = server.load("asfsdfdasfdasfdasfdasfjdklsfjdklsfjkldsjafs");
	EXPECT_FALSE(result);
}

TEST_F(TestHttpServer, webDirectoryConfigurationLocalhost)
{
	WebDirectory webDir;
	bool result = webDir.load("vhosts.d/localhost.xml");
	EXPECT_TRUE(result);
	EXPECT_EQ("/var/www/localhost/htdocs", webDir.m_path);
	EXPECT_FALSE(webDir.m_followSymLink);
	EXPECT_TRUE(webDir.m_showContent);
	EXPECT_EQ("/", webDir.m_alias);
}

TEST_F(TestHttpServer, webDirectoryConfigurationLocalhost_noFile)
{
	WebDirectory webDir;
	bool result = webDir.load("fadkfasjfdjasfldasfjldasfjldasfjl");
	EXPECT_FALSE(result);
}

TEST_F(TestHttpServer, webDirectoryConfigurationResroot)
{
	WebDirectory webDir;
	bool result = webDir.load("vhosts.d/resources.xml");
	EXPECT_TRUE(result);
	EXPECT_EQ("/var/lib/tinyhttp.d/htdocs", webDir.m_path);
	EXPECT_FALSE(webDir.m_followSymLink);
	EXPECT_FALSE(webDir.m_showContent);
	EXPECT_EQ("", webDir.m_alias);
}

TEST_F(TestHttpServer, webDirectoryConfigurations)
{
	WebDirectories webDirs;
	int result = loadWebDirectories("vhosts.d", webDirs);
	EXPECT_TRUE(result);

	const std::string localhostPath = "/var/www/localhost/htdocs";
	if (boost::filesystem::is_directory(localhostPath)) {
		bool found;
		std::for_each(webDirs.begin(), webDirs.end(), [&](const WebDirectory& webDir) {
			if (webDir.m_path == localhostPath) {
				found = true;

				EXPECT_EQ("/", webDir.m_alias);
				EXPECT_FALSE(webDir.m_followSymLink);
				EXPECT_TRUE(webDir.m_showContent);
			}
		});

		EXPECT_TRUE(found) << "Directory " + localhostPath + " exists, but server loader doesn't processed it";
	}

	const std::string resourceRootPath = "/var/lib/tinyserver/htdocs";
	if (boost::filesystem::is_directory(resourceRootPath)) {
		bool found;
		std::for_each(webDirs.begin(), webDirs.end(), [&](const WebDirectory& webDir) {
			if (webDir.m_path == resourceRootPath) {
				found = true;

				EXPECT_EQ("/", webDir.m_alias);
				EXPECT_FALSE(webDir.m_followSymLink);
				EXPECT_FALSE(webDir.m_showContent);
			}
		});

		EXPECT_TRUE(found) << "Directory " + resourceRootPath + " exists, but server loader doesn't processed it";
	}
}

TEST_F(TestHttpServer, confload)
{
	std::vector<std::string> params {"FileServer_test", "-t", "-c", "."};
	int res = app.run(params);

	EXPECT_EQ(0, res);
	EXPECT_EQ(8082, app.port());
}

TEST_F(TestHttpServer, processUrl) {
	std::string result;
	bool res;

	res = RequestHandlerFactory::processUrlSufix("/localhost", "/localhost/test", result);
	EXPECT_TRUE(res);
	EXPECT_EQ("/test", result);

	res = RequestHandlerFactory::processUrlSufix("/localhost", "/localhost", result);
	EXPECT_TRUE(res);
	EXPECT_EQ("", result);

	res = RequestHandlerFactory::processUrlSufix("/", "/localhost", result);
	EXPECT_TRUE(res);
	EXPECT_EQ("localhost", result);

	res = RequestHandlerFactory::processUrlSufix("/alias", "/badurl", result);
	EXPECT_FALSE(res);

	res = RequestHandlerFactory::processUrlSufix("", "/full_url_no_alias", result);
	EXPECT_TRUE(res);
	EXPECT_EQ("/full_url_no_alias", result);
}
