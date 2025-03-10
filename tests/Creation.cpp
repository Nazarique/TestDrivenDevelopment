#include "../Test.h"

TEST("Test can be created")
{
}

TEST_EX("Test with throw can be created", int)
{
    throw 1;
}

TEST("Test that throws unexpectedly can be created")
{
    setExpectedFailureReason("Unexpected exception thrown.");
    throw "Unexpected";
}

TEST_EX("Test that never throws unexpectedly can be created", int)
{
    setExpectedFailureReason("Expected exception type int was not thrown.");
}

TEST_EX("Test that throws wrong type can be created", int)
{
    setExpectedFailureReason("Unexpected exception thrown.");
    throw "Wrong Type";
}