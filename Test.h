#ifndef TDD_TEST_H
#define TDD_TEST_H

#include <string_view>
#include <ostream>
#include <vector>
#include <iostream>

namespace TDD
{
    // These macros generate unique names for tests using the line number:
    // 1. A class name, like Test7 if the TEST macro is on line 7.
    // 2. An instance name, like test7_inst.
    //
    // This ensures each test has a unique name, allowing multiple tests in the same file.

#define TDD_CLASS_FINAL(line) Test##line
#define TDD_CLASS_RELAY(line) TDD_CLASS_FINAL(line)
#define TDD_CLASS TDD_CLASS_RELAY(__LINE__)

#define TDD_INSTANCE_FINAL(line) test##line##_inst
#define TDD_INSTANCE_RELAY(line) TDD_INSTANCE_FINAL(line)
#define TDD_INSTANCE TDD_INSTANCE_RELAY(__LINE__)

    static std::ostream *outStream = &std::cout; // default

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
        TestBase(std::string_view name) : mName(name), mPassed(true) {}

        virtual ~TestBase() = default;

        virtual void run() = 0;

        virtual void runEx()
        {
            run();
        }

        std::string_view name() const { return mName; }

        std::string_view reason() const { return mReason; }

        bool passed() const { return mPassed; }

        std::string_view expectedReason() const { return mExpectedReason; }

        void setFailed(std::string_view reason)
        {
            mPassed = false;
            mReason = reason;
        }

        void setExpectedFailureReason(std::string_view reason)
        {
            mExpectedReason = reason;
        }

    private:
        bool mPassed;
        std::string mName;
        std::string mReason;
        std::string mExpectedReason;
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
            std::string message = "Expected exception type: ";
            message += ex.exceptionType();
            message += " was not thrown.";
            test->setFailed(message);
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
            return !test->expectedReason().empty() && test->expectedReason() == test->reason();
        }

        static bool isMissedExpectedFailure(const TestBase *test)
        {
            return !test->expectedReason().empty() && test->passed();
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
                ++counters.failed;
                *outStream << "Failed\n"
                           << test->reason() << std::endl;
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
} // namespace TDD

#define TEST_EX(testName, exceptionType)                  \
    class TDD_CLASS : public TDD::TestBase                \
    {                                                     \
    public:                                               \
        TDD_CLASS(std::string_view name) : TestBase(name) \
        {                                                 \
            TDD::getTests().push_back(this);              \
        } /* The Test constructor register itself. */     \
        void runEx() override                             \
        {                                                 \
            try                                           \
            {                                             \
                run();                                    \
            }                                             \
            catch (exceptionType const &)                 \
            {                                             \
                return;                                   \
            }                                             \
            throw TDD::MissingException(#exceptionType);  \
        }                                                 \
        void run() override;                              \
    };                                                    \
    TDD_CLASS TDD_INSTANCE(testName);                     \
    void TDD_CLASS::run()

#define TEST(testName)                                    \
    class TDD_CLASS : public TDD::TestBase                \
    {                                                     \
    public:                                               \
        TDD_CLASS(std::string_view name) : TestBase(name) \
        {                                                 \
            TDD::getTests().push_back(this);              \
        } /* The Test constructor register itself. */     \
        void run() override;                              \
    };                                                    \
    TDD_CLASS TDD_INSTANCE(testName);                     \
    void TDD_CLASS::run()
#endif // TDD_TEST_H"
