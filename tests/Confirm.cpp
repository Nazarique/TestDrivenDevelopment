#include "../Test.h"

bool isNegative(const int &value)
{
	return value < 0;
}

template <typename T>
T multipleBy2(T value)
{
	return value * 2;
}
/*
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
} */
/*
TEST("Test long confirms")
{
	long result = multipleBy2(0L);
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
} */
/*
TEST("Test long long confirms")
{
	long long result = multipleBy2(0LL);
	CONFIRM(0LL, result);

	result = multipleBy2(1LL);
	CONFIRM(2LL, result);

	result = multipleBy2(-1LL);
	CONFIRM(-2LL, result);
}

TEST("Test long long confirms failure")
{
	std::string reason = "    Expected: 0\n";
	reason += "    Actual: 2";
	setExpectedFailureReason(reason);
	long long result = multipleBy2(1LL);
	CONFIRM(0LL, result);
} */
/*
TEST("Test string confirms")
{
	std::string result = "abc";
	std::string expected = "abc";
	CONFIRM(expected, result);
}

TEST("Test string confirms failure")
{
	std::string reason = "    Expected: def\n";
	reason += "    Actual: abc";
	setExpectedFailureReason(reason);

	std::string result = "abc";
	std::string expected = "def";
	CONFIRM(expected, result);
} */

TEST("Test bool pointer dereference confirms")
{
	bool result1 = true;
	bool result2 = false;
	bool *pResult1 = &result1;
	bool *pResult2 = &result2;

	CONFIRM_TRUE(*pResult1);
	CONFIRM_FALSE(*pResult2);
}

TEST("Test string and string literal confirms")
{
	std::string result = "abc";
	CONFIRM("abc", result);
}