#include "gtest/gtest.h"

TEST(SampleTest, CaptainObvious) 
{
    EXPECT_EQ(1, 1);
}

int main(int argc, char **argv) 
{
    printf("Running tests...\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}