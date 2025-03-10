#include "../Test.h"

bool isNegative(const int &value)
{
	return value < 0;
}

long multipleBy2(long value)
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

TEST("Test long confirms")
{
	int result = multipleBy2(0L);
	CONFIRM(0L, result);

	result = multipleBy2(1L);
	CONFIRM(2L, result);

	result = multipleBy2(-1L);
	CONFIRM(-2L, result);
}

TEST("Test long confirms failure")
{
	std::string reason = "    Expected: 0\n";
	reason += "    Actual: 2";
	setExpectedFailureReason(reason);
	long result = multipleBy2(1L);
	CONFIRM(0L, result);
}