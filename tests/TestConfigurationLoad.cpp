#include <gtest/gtest.h>
#include "Configuration.h"

struct ConfigurationLoad : public ::testing::Test
{
        void SetUp()
        {
        }

        void TearDown()
        {
        }
};

TEST_F(ConfigurationLoad, init)
{
	Configuration configuration;	
}

TEST_F(ConfigurationLoad, noFile)
{
	Configuration conf;

	bool result;
	result = conf.load("/fdafasfsdfasfasfsafsdf");
	ASSERT_FALSE(result);
}
