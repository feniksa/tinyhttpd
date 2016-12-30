#include <gtest/gtest.h>
#include "FileRange.h"

TEST(TestFileRange, operations)
{
	FileRange range {0, 0, 777};
	EXPECT_EQ(1, range.length());
	EXPECT_EQ(777, range.fileSize());
	EXPECT_EQ(0, range.begin());
	EXPECT_EQ(0, range.end());

	range.begin(111);
	range.end(222);
	EXPECT_TRUE(range.validate());

	range.begin(999);
	range.end(1000);
	EXPECT_FALSE(range.validate());

	range.begin(2);
	range.end(1);
	EXPECT_FALSE(range.validate());

	range.begin(1000);
	range.end(999);
	EXPECT_FALSE(range.validate());
}

TEST(TestFileRange, toByteString)
{
	FileRange range {4, 100, 777};

	std::string result = to_string(range);
	EXPECT_EQ("bytes 4-100/777", result);
}
