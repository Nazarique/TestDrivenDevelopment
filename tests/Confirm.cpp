#include "../Test.h"

bool isNegative(const int &value)
{
	return value < 0;
}

int multipleBy2(int value)
{
	return value * 2;
}

TEST("Test bool confirms failure")
{
	std::string reason = "    Expected: true ";
	setExpectedFailureReason(reason);
	bool result = isNegative(0);
	CONFIRM_TRUE(result);
}
TEST("Test int confirms failure")
{
	std::string reason = "    Expected: 0\n";
	reason += "    Actual: 2";
	setExpectedFailureReason(reason);
	int result = multipleBy2(1);
	CONFIRM(0, result);
}