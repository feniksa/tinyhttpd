#include <gtest/gtest.h>

#include <StreamCopy.h>
#include <string>
#include <sstream>
#include <fstream>

TEST(TestStreamCopy, StreamCopy)
{
	std::string str = "abcdefg";
	std::istringstream in(str);
	std::ostringstream out;
	unsigned long long count;

	// full copy
	count = streamCopy(in, 0, str.size(), 1024, &out);
	EXPECT_EQ(str.size(), count);
	EXPECT_EQ(str, out.str());

	std::ostringstream out2;

	// partial copy
	count = streamCopy(in, 0, str.size()-1, 1024, &out2);
	EXPECT_EQ(str.size()-1, count);
	EXPECT_EQ(std::string(str.data(), str.size() - 1), out2.str());


	// empty input stream
	std::string empty;
	std::istringstream inEmpty(empty);
	std::ostringstream out3;

	// emtpy input stream
	count = streamCopy(inEmpty, 0, str.size(), 1024, &out3);
	EXPECT_EQ(0, count);

	// bad output stream
	std::ofstream badOutput;
	badOutput.open("/nonexistsdirectory", std::ios::binary);
	ASSERT_FALSE(badOutput.good());

	count = streamCopy(in, 0, str.size(), 1024, &badOutput);
	EXPECT_EQ(0, count);

	// bad input stream
	std::ostringstream out4;
	std::ifstream badInput;
	badInput.open("/nonexistsdirectory", std::ios::binary);
	ASSERT_FALSE(badInput.good());

	count = streamCopy(badInput, 0, str.size(), 1024, &out4);
	EXPECT_EQ(0, count);
}
