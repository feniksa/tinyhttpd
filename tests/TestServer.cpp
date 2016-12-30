#include <gtest/gtest.h>

#include "HttpServer.h"
#include <ios>
#include <thread>

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/Net/HTTPCredentials.h>
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include <Poco/File.h>
#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <mutex>
#include <condition_variable>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;

namespace
{
	struct MutexBlocker : public HttpServer::Blocker
	{
		std::mutex mutex;
		std::condition_variable cv;

		void unlock()
		{
			{
				std::lock_guard<std::mutex> lk(mutex);				;
				std::cout << "main() signals data ready for processing\n";
			}
			cv.notify_one();
		}

		virtual void wait(HttpServer*)
		{
			std::unique_lock<std::mutex> lk(mutex);
			cv.wait(lk);
		}
	};


const int TestPort = 9999;
const std::string IndexContent = "<html><title>Test</title><body>Test</body></html>";

static std::string random_string( size_t length )
{
	auto randchar = []() -> char
	{
		const char charset[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[ rand() % max_index ];
	};
	std::string str(length,0);
	std::generate_n( str.begin(), length, randchar );
	return str;
}

}

struct TestServer : public ::testing::Test
{
	sigset_t x;
	Poco::Path tempDir;

	std::shared_ptr<MutexBlocker> blocker;
	HttpServer app;
	std::thread server;

	const Poco::URI indexUri() {
		return Poco::URI("http://localhost:" + std::to_string(TestPort) + tempDir.toString() + "/index.html");
	}
	Poco::Path indexPath;

    TestServer()
	: blocker(new MutexBlocker),
	  app(blocker)
	{
		sigemptyset (&x);
		sigaddset(&x, SIGINT);
	}


	void SetUp()
	{
		srand(static_cast<unsigned int>(time(0)));

		//ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &x, NULL));
		//ASSERT_EQ(0, pthread_sigmask(SIG_BLOCK, &x, NULL));

		tempDir = Poco::Path(Poco::Path::temp(), "FileDirTest_" + random_string(10));

		Poco::File file(tempDir);
		bool isCreated = file.createDirectory();
		ASSERT_TRUE(isCreated);

		isCreated = createIndexHtml();
		ASSERT_TRUE(isCreated);

		start_server();
	}

	void TearDown()
	{
		stop_server();

		//ASSERT_EQ(0, sigprocmask(SIG_UNBLOCK, &x, NULL));

		Poco::File dir(tempDir);
		dir.remove(true);
	}

	bool createIndexHtml()
	{
		indexPath = Poco::Path(tempDir, "index.html");
		std::ofstream out;

		out.open(indexPath.toString());
		if (!out.is_open())
			return false;


		out << IndexContent;

		out.close();

		return true;
	}

	std::string getContent(const Poco::URI uri, std::map<std::string, std::string> params = std::map<std::string, std::string>(), Poco::Net::HTTPResponse::HTTPStatus returnStatus = Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK)
	{
		std::ostringstream out;
		std::string path(uri.getPathAndQuery());

		HTTPClientSession session(uri.getHost(), uri.getPort());
		HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		HTTPResponse response;

		for (const auto& kv : params) {
			request.set(kv.first, kv.second);
		}

		session.sendRequest(request);
		std::istream& rs = session.receiveResponse(response);
		std::cout << response.getStatus() << " " << response.getReason() << std::endl;

		EXPECT_EQ(response.getStatus(), returnStatus) << response.getStatus();

		StreamCopier::copyStream(rs, out);

		return out.str();
	}

	void start_server()
	{
		server = std::thread([this]{
			int res;
			std::vector<std::string> params {"FileServer_test", "-p", std::to_string(TestPort), tempDir.toString()};

			std::ios_base::sync_with_stdio(false);

			res = app.run(params);
			return res;
		});

		while(!app.initialized()) {
			usleep(500); // <-- wait while server initialized
		}
	}


	void stop_server()
	{
		//app.terminate();
		blocker->unlock();
		server.join();
	}
};

TEST_F(TestServer, index_html)
{
	std::string index = getContent(indexUri());
	EXPECT_EQ(IndexContent, index);
}

static std::string makeRange(const unsigned int RangeBegin, const unsigned int RangeEnd)
{
	return "bytes=" + std::to_string(RangeBegin) + "-" + std::to_string(RangeEnd);
}

TEST_F(TestServer, range_full)
{
	const unsigned int RangeBegin = 1;
	const unsigned int RangeEnd = 5;
	std::string partialContent;

	ASSERT_TRUE(RangeBegin < IndexContent.size());
	ASSERT_TRUE(RangeEnd < IndexContent.size());

	std::copy_n(IndexContent.begin() + RangeBegin, RangeEnd - RangeBegin + 1, std::back_inserter(partialContent));

	std::string index = getContent(indexUri(), { std::make_pair("Range", makeRange(RangeBegin, RangeEnd)) }, Poco::Net::HTTPResponse::HTTPStatus::HTTP_PARTIAL_CONTENT );
	EXPECT_EQ(partialContent, index);
}

TEST_F(TestServer, range_begin)
{
	const unsigned int DeepRangeBegin = 8;
	std::string partialContent;

	ASSERT_TRUE(DeepRangeBegin < IndexContent.size());

	std::copy_n(IndexContent.begin() + DeepRangeBegin, IndexContent.size() - DeepRangeBegin, std::back_inserter(partialContent));

	std::string index = getContent(indexUri(), { std::make_pair("Range", "bytes="+std::to_string(DeepRangeBegin)+"-") }, Poco::Net::HTTPResponse::HTTPStatus::HTTP_PARTIAL_CONTENT );
	EXPECT_EQ(partialContent, index);
}

TEST_F(TestServer, range_end)
{
	const unsigned int DeepRangeEnd = 10;
	std::string partialContent;

	 ASSERT_TRUE(DeepRangeEnd < IndexContent.size());

	std::copy_n(IndexContent.end() - DeepRangeEnd, DeepRangeEnd, std::back_inserter(partialContent));
	std::string index = getContent(indexUri(), { std::make_pair("Range", "bytes=-"+std::to_string(DeepRangeEnd)) }, Poco::Net::HTTPResponse::HTTPStatus::HTTP_PARTIAL_CONTENT );
	EXPECT_EQ(partialContent, index);
}

TEST_F(TestServer, lastModified)
{
	std::ostringstream out;
	std::string path(indexUri().getPathAndQuery());

	HTTPClientSession session(indexUri().getHost(), indexUri().getPort());
	HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
	HTTPResponse response;

	session.sendRequest(request);
	std::istream& rs = session.receiveResponse(response);
	std::cout << response.getStatus() << " " << response.getReason() << std::endl;

	EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK) << response.getStatus();
	StreamCopier::copyStream(rs, out);

	EXPECT_EQ(IndexContent, out.str());

	int zoneOffset;
	Poco::DateTime lastModified = Poco::DateTimeParser::parse(Poco::DateTimeFormat::HTTP_FORMAT,  response.get("Last-Modified"), zoneOffset);
	EXPECT_EQ(Poco::File(indexPath).getLastModified(), lastModified.timestamp());
}

TEST_F(TestServer, cache_notModified)
{
	std::ostringstream out;
	std::string path(indexUri().getPathAndQuery());

	HTTPClientSession session(indexUri().getHost(), indexUri().getPort());
	HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
	request.set("If-Modified-Since", Poco::DateTimeFormatter::format(Poco::File(indexPath).getLastModified(), Poco::DateTimeFormat::HTTP_FORMAT));
	HTTPResponse response;

	session.sendRequest(request);
	std::istream& rs = session.receiveResponse(response);
	std::cout << response.getStatus() << " " << response.getReason() << std::endl;

	EXPECT_EQ(response.getStatus(), Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_MODIFIED) << response.getStatus();
	StreamCopier::copyStream(rs, out);

	EXPECT_TRUE(out.str().empty());
}
