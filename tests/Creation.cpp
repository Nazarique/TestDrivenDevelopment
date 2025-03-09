#include "../Test.h"

TEST("Test can be created")
{
}

TEST_EX("Test with throw can be created", int)
{
    throw 1;
}

TEST_EX("Test that never throws can be created", int)
{
}