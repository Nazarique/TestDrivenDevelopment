#include "../Test.h"

bool isNegative(const int &value)
{
    return value < 0;
}

int multipleBy2(const int &value)
{
    return value * 2;
}

TEST("Test bool confirms failure")
{
    bool result = isNegative(0);
    CONFIRM_TRUE(result);
}
TEST("Test int confirms failure")
{
    int result = multipleBy2(1);
    CONFIRM(0, result);
}