#include <gtest/gtest.h>

#include "HttpHeaderParse.h"

const unsigned long long BeginRange = 111;
const unsigned long long EndRange = 777;
const unsigned long long FileSize = 999;

TEST(TestHeaderParsers, bytesRange)
{
	bool result;
	FileRange range {BeginRange, EndRange, FileSize};

	std::string emptyIntput;

	result = parse::bytesRange(emptyIntput, &range);
	EXPECT_TRUE(result);
	EXPECT_EQ(BeginRange, range.begin());
	EXPECT_EQ(EndRange, range.end());

	// parse similar bytes range
	std::string input = "bytes=123-456";
	FileRange range2 {BeginRange, EndRange, FileSize};
	result = parse::bytesRange(input, &range2);
	EXPECT_TRUE(result);
	EXPECT_EQ(123, range2.begin());
	EXPECT_EQ(456, range2.end());

	// only begin range spec
	std::string input3 = "bytes=123-";
	FileRange range3 {BeginRange, EndRange, FileSize};
	result = parse::bytesRange(input3, &range3);
	EXPECT_TRUE(result);
	EXPECT_EQ(123, range3.begin());
	EXPECT_EQ(EndRange, range3.end());

	// only end range spec
	std::string input4 = "bytes=-456";
	FileRange range4 {BeginRange, EndRange, FileSize};
	result = parse::bytesRange(input4, &range4);
	EXPECT_TRUE(result);
	EXPECT_EQ(EndRange - 456 + 1, range4.begin());
	EXPECT_EQ(EndRange, range4.end());

	// bad inputs without bytes
	std::string input5 = "fdasfsdfas";
	FileRange range5 {BeginRange, EndRange, FileSize};
	result = parse::bytesRange(input5, &range5);
	EXPECT_FALSE(result);
	EXPECT_EQ(BeginRange, range5.begin());
	EXPECT_EQ(EndRange, range5.end());

	std::string input6 = "bytes=";
	FileRange range6 {BeginRange, EndRange, FileSize};
	result = parse::bytesRange(input6, &range6);
	EXPECT_FALSE(result);
	EXPECT_EQ(BeginRange, range6.begin());
	EXPECT_EQ(EndRange, range6.end());

	std::string input7 = "bytes=-1--2";
	FileRange range7 {BeginRange, EndRange, FileSize};
	result = parse::bytesRange(input7, &range7);
	EXPECT_FALSE(result);
	EXPECT_EQ(BeginRange, range7.begin());
	EXPECT_EQ(EndRange, range7.end());

	std::string input8 = "bytes=asfsaffsafsfsdf";
	FileRange range8 {BeginRange, EndRange, FileSize};
	result = parse::bytesRange(input8, &range8);
	EXPECT_FALSE(result);
	EXPECT_EQ(BeginRange, range8.begin());
	EXPECT_EQ(EndRange, range8.end());
}
