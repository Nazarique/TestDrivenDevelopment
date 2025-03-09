#ifndef TDD_TEST_H
#define TDD_TEST_H

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

#include <string_view>
#include <ostream>
#include <vector>
#include <iostream>

namespace TDD
{
    static std::ostream *outStream = &std::cout;
    // default

    class MissingException
    {
    public:
        MissingException(std::string_view exType) : mExType(exType) {}

        std::string_view exType() const { return mExType; }

    private:
        std::string mExType;
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

        void setFailed(std::string_view reason)
        {
            mPassed = false;
            mReason = reason;
        }

    private:
        std::string mName;
        std::string mReason;
        bool mPassed;
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

    inline void summaryTests(const int &testsPassed, const int &testsFailed)
    {
        (*outStream) << "\n-------------------------" << std::endl;
        if (testsFailed == 0)
        {
            (*outStream) << "All tests passed." << std::endl;
        }
        else
        {
            (*outStream) << "Tests passed: " << testsPassed
                         << "\nTests failed: " << testsFailed
                         << std::endl;
        }
    }

    inline void runTests()
    {
        int testsPassed = 0, testsFailed = 0;
        (*outStream) << "Running " << getTests().size()
                     << " tests\n";
        for (auto *test : TDD::getTests())
        {
            (*outStream) << "--------\n"
                         << test->name() << std::endl;
            try
            {
                test->runEx();
            }
            catch (MissingException const &ex)
            {
                std::string message = "Expected exception type: ";
                message += ex.exType();
                message += " was not thrown.";
                test->setFailed(message);
            }
            catch (...)
            {
                test->setFailed("Unexpected exception thrown.");
            }
            if (test->passed())
            {
                testsPassed++;
                (*outStream) << "Passed" << std::endl;
            }
            else
            {
                testsFailed++;
                (*outStream) << "Failed" << std::endl
                             << test->reason() << std::endl;
            }
        }
        summaryTests(testsPassed, testsFailed);
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
