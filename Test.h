#ifndef TDD_TEST_H
#define TDD_TEST_H

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
    class TestRunner;

    inline void setOutStream(std::ostream &os);
    inline std::vector<TestBase *> &getTests();
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
            mReason += expected ? "true " : "false ";
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
        TestBase(std::string_view name)
            : mName(name),
              mPassed(true), mConfirmLocation(-1)
        {
            getTests().push_back(this);
        }

        virtual ~TestBase() = default;

        virtual void run() = 0;

        virtual void runEx()
        {
            run();
        }

        std::string_view name() const { return mName; }

        std::string_view reason() const { return mReason; }

        bool passed() const { return mPassed; }

        int confirmLocation() const { return mConfirmLocation; }

        std::string_view expectedReason() const { return mExpectedReason; }

        void setFailed(std::string_view reason, int confirmLocation = -1)
        {
            mPassed = false;
            mReason = reason;
            mConfirmLocation = confirmLocation;
        }

        void setExpectedFailureReason(std::string_view reason)
        {
            mExpectedReason = reason;
        }

    private:
        bool mPassed;
        int mConfirmLocation;
        std::string mName;
        std::string mReason;
        std::string mExpectedReason;
    };

    template <typename ExceptionT>
    class TestExBase : public TestBase
    {
    public:
        TestExBase(std::string_view name, std::string_view exceptionName)
            : TestBase(name), mExceptionName(exceptionName) {}

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

    class TestRunner
    {
    public:
        static void runAllTests(std::vector<TestBase *> allTests)
        {
            TestCounters counters;
            *outStream << "Running " << allTests.size() << " tests\n";

            for (auto *test : allTests)
            {
                *outStream << "-------------------------\n"
                           << test->name() << std::endl;
                handleTest(test);
                updateTestCounters(test, counters);
            }

            printTestSummary(counters);
        }

    private:
        struct TestCounters
        {
            int passed = 0;
            int failed = 0;
            int missedFailures = 0;
        };

        static void printTestSummary(const TestCounters &counters)
        {
            *outStream << "-------------------------" << std::endl;
            if (counters.failed == 0)
            {
                *outStream << "All tests passed." << std::endl;
            }
            else
            {
                *outStream << "Tests passed: " << counters.passed
                           << "\nTests failed: " << counters.failed
                           << "\nMissed failures: " << counters.missedFailures
                           << std::endl;
            }
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

        static void handleTest(TestBase *test)
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

        static bool isExpectedFailure(const TestBase *test)
        {
            return (not test->expectedReason().empty()) && test->expectedReason() == test->reason();
        }

        static bool isMissedExpectedFailure(const TestBase *test)
        {
            return not test->expectedReason().empty();
        }

        static void verifyPassedTest(const TestBase *test, TestCounters &counters)
        {
            if (isMissedExpectedFailure(test))
            {
                ++counters.missedFailures;
                *outStream << "Missed expected failure\n"
                           << "Test passed but was expected to fail."
                           << std::endl;
            }
            else
            {
                ++counters.passed;
                *outStream << "Passed" << std::endl;
            }
        }

        static void verifyFailedTest(const TestBase *test, TestCounters &counters)
        {
            if (isExpectedFailure(test))
            {
                ++counters.passed;
                *outStream << "Expected failure\n"
                           << test->reason() << std::endl;
            }
            else
            {
                if (test->confirmLocation() != -1)
                {
                    *outStream << "Failed confirm on line "
                               << test->confirmLocation() << std::endl;
                }
                else
                {
                    *outStream << "Failed\n";
                }
                ++counters.failed;
                *outStream << test->reason() << std::endl;
            }
        }

        static void updateTestCounters(TestBase *test, TestCounters &counters)
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

    inline void setOutStream(std::ostream &os)
    {
        outStream = &os;
    }

    inline std::vector<TestBase *> &getTests()
    {
        static std::vector<TestBase *> tests;

        return tests;
    }

    inline void runTests()
    {
        TestRunner::runAllTests(getTests());
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

#define TEST(testName)                         \
    namespace                                  \
    {                                          \
        class TDD_CLASS : public TDD::TestBase \
        {                                      \
        public:                                \
            TDD_CLASS(std::string_view name)   \
                : TestBase(name)               \
            {                                  \
            }                                  \
            void run() override;               \
        };                                     \
    } /* end of unnamed namespace */           \
    TDD_CLASS TDD_INSTANCE(testName);          \
    void TDD_CLASS::run()

#define TEST_EX(testName, exceptionType)                        \
    namespace                                                   \
    {                                                           \
        class TDD_CLASS : public TDD::TestExBase<exceptionType> \
        {                                                       \
        public:                                                 \
            TDD_CLASS(std::string_view name,                    \
                      std::string_view exceptionName)           \
                : TestExBase(name, exceptionName)               \
            {                                                   \
            }                                                   \
            void run() override;                                \
        };                                                      \
    } /* end of unnamed namespace */                            \
    TDD_CLASS TDD_INSTANCE(testName, #exceptionType);           \
    void TDD_CLASS::run()

#define CONFIRM(expected, actual) \
    TDD::confirm(expected, actual, __LINE__);

#define CONFIRM_FALSE(actual) \
    TDD::confirm(false, actual, __LINE__);

#define CONFIRM_TRUE(actual) \
    TDD::confirm(true, actual, __LINE__);
#endif // TDD_TEST_H"