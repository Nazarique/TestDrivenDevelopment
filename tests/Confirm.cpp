#include "../Test.h"

bool isPassingGrade(const int &value)
{
    if (value < 60)
    {
        return false;
    }

    return true;
}

TEST("Test will pass without any confirms")
{
}

TEST("Test passing grades")
{
    bool result = isPassingGrade(0);
    if (result)
    {
        throw TDD::BoolConfirmException(false, 22);
    }
    result = isPassingGrade(100);
    if (result)
    {
        throw TDD::BoolConfirmException(true, 27);
    }
}