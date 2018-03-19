#include "gtest/gtest.h"

#include "utils.hpp"

TEST(ExpandCyclicInequality, SumOfIA) 
{
    string inequality = "[sum IA]";
    expand_cyclic_sums(inequality);
    EXPECT_EQ("((IA) + (IB) + (IC))", inequality);
}

TEST(ExpandCyclicInequality, SumOfHalfSines) 
{
    string inequality = "[sum sinA2]";
    expand_cyclic_sums(inequality);
    EXPECT_EQ("((sinA2) + (sinB2) + (sinC2))", inequality);
}

TEST(ExpandCyclicInequality, ProductStartingFromB) 
{
    string inequality = "[prod b^2]";
    expand_cyclic_products(inequality);
    EXPECT_EQ("((b^2) * (c^2) * (a^2))", inequality);
}

TEST(ExpandCyclicInequality, ComplexSumsAndProducts) 
{
    string inequality = "[sum sqrt((b + c - a) / a)] + [prod sqrt((c * IB) / (GA * NC))]";
    expand_cyclic_products(inequality);
    expand_cyclic_sums(inequality);
    EXPECT_EQ("((sqrt((b + c - a) / a)) + (sqrt((c + a - b) / b)) + (sqrt((a + b - c) / c))) "
              "+ ((sqrt((c * IB) / (GA * NC))) * (sqrt((a * IC) / (GB * NA))) * (sqrt((b * IA) "
              "/ (GC * NB))))", inequality);
}

int main(int argc, char **argv) 
{
    printf("Running tests...\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}