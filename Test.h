#ifndef TDD_TEST_H
#define TDD_TEST_H

#include <map>
#include <string_view>
#include <ostream>
#include <vector>
#include <iostream>

namespace TDD
{
    static std::ostream *outStream = &std::cout; // default

    // Forward declarations
    class ConfirmException;
    class ActualConfirmException;
    class BoolConfirmException;
    class MissingException;
    class TestBase;
    class Runner;
    class Test;
    class TestSuite;

    inline void setOutStream(std::ostream &os);
    inline std::map<std::string, std::vector<Test *>> &getTests();
    inline std::map<std::string, std::vector<TestSuite *>> &getTestSuites();
    inline void addTest(std::string_view suiteName, Test *test);
    inline void addTestSuite(std::string_view suiteName, TestSuite *suite);
    inline void runTests();

    class ConfirmException
    {
    public:
        ConfirmException(int line)
            : mLine(line) {}

        virtual ~ConfirmException() = default;

        std::string_view reason() const { return mReason; }

        int line() const { return mLine; }

    protected:
        int mLine;
        std::string mReason;
    };

    class ActualConfirmException : public ConfirmException
    {
    public:
        ActualConfirmException(std::string_view expected, std::string_view actual, int line)
            : ConfirmException(line),
              mExpected(expected),
              mActual(actual)
        {
            formatReason();
        }

    private:
        void formatReason()
        {
            mReason += "    Expected: " + mExpected + "\n";
            mReason += "    Actual  : " + mActual;
        }
        int mLine;
        std::string mExpected;
        std::string mActual;
    };

    class BoolConfirmException : public ConfirmException
    {
    public:
        BoolConfirmException(bool expected, int line) : ConfirmException(line)
        {
            mReason += "    Expected: ";
            mReason += expected ? "true" : "false";
        }
    };

    class MissingException
    {
    public:
        MissingException(std::string_view exceptionType) : mExceptionType(exceptionType) {}

        std::string_view exceptionType() const { return mExceptionType; }

    private:
        std::string mExceptionType;
    };

    class TestBase
    {
    public:
        TestBase(std::string_view name, std::string_view suiteName)
            : mName(name),
              mSuiteName(suiteName),
              mPassed(true),
              mConfirmLocation(-1) {}

        virtual ~TestBase() = default;

        std::string_view name() const { return mName; }

        std::string_view suiteName() const { return mSuiteName; }

        std::string_view reason() const { return mReason; }

        bool passed() const { return mPassed; }

        int confirmLocation() const { return mConfirmLocation; }

        void setFailed(std::string_view reason, int confirmLocation = -1)
        {
            mPassed = false;
            mReason = reason;
            mConfirmLocation = confirmLocation;
        }

    private:
        std::string mName;
        std::string mSuiteName;
        std::string mReason;
        bool mPassed;
        int mConfirmLocation;
    };

    class Test : public TestBase
    {
    public:
        Test(std::string_view name, std::string_view suiteName)
            : TestBase(name, suiteName)
        {
            addTest(suiteName, this);
        }
        virtual void runEx()
        {
            run();
        }
        virtual void run() = 0;

        std::string_view expectedReason() const { return mExpectedReason; }

        void setExpectedFailureReason(std::string_view reason)
        {
            mExpectedReason = reason;
        }

    private:
        std::string mName;
        std::string mSuiteName;
        std::string mExpectedReason;
    };

    template <typename ExceptionT>
    class TestEx : public Test
    {
    public:
        TestEx(std::string_view name, std::string_view suiteName, std::string_view exceptionName)
            : Test(name, suiteName), mExceptionName(exceptionName) {}

        void runEx() override
        {
            try
            {
                run();
            }
            catch (ExceptionT const &)
            {
                return;
            }
            throw MissingException(mExceptionName);
        }

    private:
        std::string_view mExceptionName;
    };

    class TestSuite : public TestBase
    {
    public:
        TestSuite(std::string_view name, std::string_view suiteName)
            : TestBase(name, suiteName)
        {
            addTestSuite(suiteName, this);
        }
        virtual void suiteSetup() = 0;
        virtual void suiteTeardown() = 0;
    };

    class Runner
    {
    private:
        struct TestCounters
        {
            int passed = 0;
            int failed = 0;
            int missedFailures = 0;
        };

    public:
        static int runAllTests()
        {
            TestCounters counters;
            *outStream << "Running " << getTests().size() << " test suites\n";

            for (auto const &[suiteName, tests] : getTests())
            {
                printSuiteHeader(suiteName);

                if (isSuiteNotFound(suiteName))
                {
                    return ++counters.failed;
                }

                if (!runSuiteSetup(suiteName, counters))
                {
                    continue;
                }

                for (auto *test : tests)
                {
                    runTest(test, counters);
                }

                runSuiteTeardown(suiteName, counters);
            }

            printTestSummary(counters);
            return counters.failed;
        }

    private:
        static void printSuiteHeader(const std::string_view suiteName)
        {
            std::string suiteDisplayName = "Suite: ";
            suiteDisplayName += suiteName.empty() ? "Single Tests" : suiteName;

            *outStream << "------------------ " << suiteDisplayName << std::endl;
        }

        static void printTestSummary(const TestCounters &counters)
        {
            *outStream << "-------------------------" << std::endl;

            *outStream << "Tests passed: " << counters.passed
                       << "\nTests failed: " << counters.failed;

            if (counters.missedFailures != 0)
            {
                *outStream << "\nMissed failures: " << counters.missedFailures;
            }
            *outStream << std::endl;
        }

        static bool isSuiteNotFound(const std::string &suiteName)
        {
            if (not suiteName.empty() && not getTestSuites().contains(suiteName))
            {
                *outStream << "Test suite is not found. Exiting test application." << std::endl;
                return true;
            }
            return false;
        }

        static bool runSuiteSetup(const std::string &suiteName, TestCounters &counters)
        {
            if (not suiteName.empty() && not runSuite(true, suiteName, counters))
            {
                *outStream << "Test suite setup failed. Skipping tests in suite." << std::endl;
                return false;
            }
            return true;
        }

        static bool runSuiteTeardown(const std::string &suiteName, TestCounters &counters)
        {
            if (not suiteName.empty() && not runSuite(false, suiteName, counters))
            {
                *outStream << "Test suite teardown failed. Skipping tests in suite." << std::endl;
                return false;
            }
            return true;
        }

        static void runTest(Test *test, TestCounters &counters)
        {
            *outStream << "------------ Test: " << test->name() << std::endl;
            handleTest(test);
            updateTestCounters(test, counters);
        }

        static bool runSuite(bool setup, std::string const &name, TestCounters &counters)
        {
            bool result = true;
            for (auto &suite : getTestSuites()[name])
            {
                if (setup)
                {
                    *outStream << "------------ Setup: ";
                }
                else
                {
                    *outStream << "------------ Teardown: ";
                }
                *outStream << suite->name() << std::endl;
                handleSuite(suite, setup);
                if (isSuiteFailed(suite, counters))
                {
                    return false;
                }
            }
            return result;
        }

        static void handleMissingException(TestBase *test, const MissingException &ex)
        {
            // Expected exception type int was not thrown
            std::string message = "Expected exception type ";
            message += ex.exceptionType();
            message += " was not thrown.";
            test->setFailed(message);
        }

        static void handleConfirmException(TestBase *test, const ConfirmException &ex)
        {
            test->setFailed(ex.reason(), ex.line());
        }

        static void handleUnexpectedException(TestBase *test)
        {
            test->setFailed("Unexpected exception thrown.");
        }

        static void handleTest(Test *test)
        {
            try
            {
                test->runEx();
            }
            catch (const ConfirmException &ex)
            {
                handleConfirmException(test, ex);
            }
            catch (const MissingException &ex)
            {
                handleMissingException(test, ex);
            }
            catch (...)
            {
                handleUnexpectedException(test);
            }
        }

        static void handleSuite(TestSuite *suite, bool setup)
        {
            try
            {
                if (setup)
                {
                    suite->suiteSetup();
                }
                else
                {
                    suite->suiteTeardown();
                }
            }
            catch (const ConfirmException &ex)
            {
                handleConfirmException(suite, ex);
            }
            catch (...)
            {
                handleUnexpectedException(suite);
            }
        }

        static bool isExpectedFailure(const Test *test)
        {
            return (not test->expectedReason().empty()) && test->expectedReason() == test->reason();
        }

        static bool isMissedExpectedFailure(const Test *test)
        {
            return not test->expectedReason().empty();
        }

        static void verifyPassedTest(const Test *test, TestCounters &counters)
        {
            if (isMissedExpectedFailure(test))
            {
                ++counters.missedFailures;
                *outStream << "Missed expected failure\n"
                           << "Test passed but was expected to fail." << std::endl;
            }
            else
            {
                ++counters.passed;
                *outStream << "Passed" << std::endl;
            }
        }

        static void verifyConfirmLocation(const TestBase *test, TestCounters &counters)
        {
            ++counters.failed;
            if (test->confirmLocation() != -1)
            {
                *outStream << "Failed confirm on line " << test->confirmLocation() << std::endl;
            }
            else
            {
                *outStream << "Failed\n";
            }
            *outStream << test->reason() << std::endl;
        }

        static void verifyFailedTest(const Test *test, TestCounters &counters)
        {
            if (isExpectedFailure(test))
            {
                ++counters.passed;
                *outStream << "Expected failure\n"
                           << test->reason() << std::endl;
            }
            else
            {
                verifyConfirmLocation(test, counters);
            }
        }

        static bool isSuiteFailed(TestSuite *suite, TestCounters &counters)
        {
            if (not suite->passed())
            {
                verifyConfirmLocation(suite, counters);
                return true;
            }
            ++counters.passed;
            *outStream << "Passed" << std::endl;
            return false;
        }

        static void updateTestCounters(Test *test, TestCounters &counters)
        {
            if (test->passed())
            {
                verifyPassedTest(test, counters);
            }
            else
            {
                verifyFailedTest(test, counters);
            }
        }
    };

    template <typename T>
    class SetupAndTeardown : public T
    {

    public:
        SetupAndTeardown(/* args */)
        {
            T::setup();
        }
        ~SetupAndTeardown()
        {
            T::teardown();
        }
    };

    template <typename T>
    class TestSuiteSetupAndTeardown : public T, public TestSuite
    {
    public:
        TestSuiteSetupAndTeardown(std::string_view name, std::string_view suite)
            : TestSuite(name, suite) {}

        void suiteSetup() override { T::setup(); }

        void suiteTeardown() override { T::teardown(); }
    };

    inline void setOutStream(std::ostream &os)
    {
        outStream = &os;
    }

    inline std::map<std::string, std::vector<Test *>> &getTests()
    {
        static std::map<std::string, std::vector<Test *>> tests;

        return tests;
    }

    inline std::map<std::string, std::vector<TestSuite *>> &getTestSuites()
    {
        static std::map<std::string, std::vector<TestSuite *>> suites;

        return suites;
    }

    inline void addTest(std::string_view suiteName, Test *test)
    {
        std::string name(suiteName);
        if (not getTests().contains(name))
        {
            getTests().try_emplace(name, std::vector<Test *>());
        }
        getTests()[name].push_back(test);
    }

    inline void addTestSuite(std::string_view suiteName, TestSuite *suite)
    {
        std::string name(suiteName);
        if (not getTestSuites().contains(name))
        {
            getTestSuites().try_emplace(name, std::vector<TestSuite *>());
        }
        getTestSuites()[name].push_back(suite);
    }

    inline void runTests()
    {
        Runner::runAllTests();
    }

    inline void confirm(bool expected, bool actual, int line)
    {
        if (actual != expected)
        {
            throw TDD::BoolConfirmException(expected, line);
        }
    }

    // overloaded to string_view
    inline void confirm(std::string_view expected, std::string_view actual, int line)
    {
        if (actual != expected)
        {
            throw TDD::ActualConfirmException(expected, actual, line);
        }
    }

    // overloaded to strings
    inline void confirm(std::string const &expected, std::string const &actual, int line)
    {
        confirm(std::string_view(expected), std::string_view(actual), line);
    }

    // overloaded to float
    inline void confirm(float expected, float actual, int line)
    {
        if (actual < (expected - 0.0001f) ||
            actual > (expected + 0.0001f))
        {
            throw TDD::ActualConfirmException(std::to_string(expected), std::to_string(actual), line);
        }
    }

    // overloaded to double
    inline void confirm(double expected, double actual, int line)
    {
        if (actual < (expected - 0.000001f) ||
            actual > (expected + 0.000001f))
        {
            throw TDD::ActualConfirmException(std::to_string(expected), std::to_string(actual), line);
        }
    }

    // overloaded to long double
    inline void confirm(long double expected, long double actual, int line)
    {
        if (actual < (expected - 0.000001) ||
            actual > (expected + 0.000001))
        {
            throw TDD::ActualConfirmException(std::to_string(expected), std::to_string(actual), line);
        }
    }

    template <typename T>
    void confirm(T const &expected, T const &actual, int line)
    {
        if (actual != expected)
        {
            throw TDD::ActualConfirmException(std::to_string(expected), std::to_string(actual), line);
        }
    }
} // namespace TDD

// These macros generate unique names for tests using the line number:
// 1. A class name, like Test7 if the TEST macro is on line 7.
// 2. An instance name, like test7_inst.
//
// This ensures each test has a unique name, allowing multiple tests in the same file.

#define TDD_CLASS_FINAL(line) Test##line
#define TDD_CLASS_RELAY(line) TDD_CLASS_FINAL(line)
#define TDD_CLASS TDD_CLASS_RELAY(__LINE__)

#define TDD_INSTANCE_FINAL(line) test##line
#define TDD_INSTANCE_RELAY(line) TDD_INSTANCE_FINAL(line)
#define TDD_INSTANCE TDD_INSTANCE_RELAY(__LINE__)

#define TEST(testName)                       \
    namespace                                \
    {                                        \
        class TDD_CLASS : public TDD::Test   \
        {                                    \
        public:                              \
            TDD_CLASS(std::string_view name) \
                : Test(name, "") {}          \
            void run() override;             \
        };                                   \
    } /* end of unnamed namespace */         \
    TDD_CLASS TDD_INSTANCE(testName);        \
    void TDD_CLASS::run()

#define TEST_EX(testName, exceptionType)                    \
    namespace                                               \
    {                                                       \
        class TDD_CLASS : public TDD::TestEx<exceptionType> \
        {                                                   \
        public:                                             \
            TDD_CLASS(std::string_view name,                \
                      std::string_view exceptionName)       \
                : TestEx(name, "", exceptionName) {}        \
            void run() override;                            \
        };                                                  \
    } /* end of unnamed namespace */                        \
    TDD_CLASS TDD_INSTANCE(testName, #exceptionType);       \
    void TDD_CLASS::run()

#define TEST_SUITE(testName, suiteName)                              \
    namespace                                                        \
    {                                                                \
        class TDD_CLASS : public TDD::Test                           \
        {                                                            \
        public:                                                      \
            TDD_CLASS(std::string_view name, std::string_view suite) \
                : Test(name, suite) {}                               \
            void run() override;                                     \
        };                                                           \
    } /* end of unnamed namespace */                                 \
    TDD_CLASS TDD_INSTANCE(testName, suiteName);                     \
    void TDD_CLASS::run()

#define TEST_SUITE_EX(testName, suiteName, exceptionType)                                            \
    namespace                                                                                        \
    {                                                                                                \
        class TDD_CLASS : public TDD::TestEx<exceptionType>                                          \
        {                                                                                            \
        public:                                                                                      \
            TDD_CLASS(std::string_view name, std::string_view suite, std::string_view exceptionName) \
                : TestEx(name, suite, exceptionName) {}                                              \
            void run() override;                                                                     \
        };                                                                                           \
    } /* end of unnamed namespace */                                                                 \
    TDD_CLASS TDD_INSTANCE(testName, suiteName, #exceptionType);                                     \
    void TDD_CLASS::run()

#define CONFIRM(expected, actual) \
    TDD::confirm(expected, actual, __LINE__);

#define CONFIRM_FALSE(actual) \
    TDD::confirm(false, actual, __LINE__);

#define CONFIRM_TRUE(actual) \
    TDD::confirm(true, actual, __LINE__);
#endif // TDD_TEST_H"