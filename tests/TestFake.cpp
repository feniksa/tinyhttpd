#include <gtest/gtest.h>

struct TestFake : public ::testing::Test
{
        void SetUp()
        {
        }

        void TearDown()
        {
        }
};

TEST_F(TestFake, empty)
{
	
}
