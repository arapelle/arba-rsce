#include <rsce/rsce.hpp>
#include <gtest/gtest.h>
#include <cstdlib>

TEST(rsce_tests, basic_test)
{
    ASSERT_EQ(module_name(), "rsce");
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
