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
#include <string>
#include <vector>
#include <iostream>

namespace TDD
{
    class TestBase
    {
    public:
        TestBase(std::string_view name) : mName(name), mPassed(true) {}

        virtual ~TestBase() = default;

        virtual void run() = 0;

        std::string_view name() const { return mName; }

        std::string_view reason() const { return mReason; }

        bool passed() const { return mPassed; }

        void setFailed(std::string_view reason)
        {
            mPassed = false;
            mReason = reason;
        }

    public:
        std::string mName;
        std::string mReason;
        bool mPassed;
    };

    inline std::vector<TestBase *> &getTests()
    {
        static std::vector<TestBase *> tests;
        return tests;
    }

    inline void runTests()
    {
        for (auto *test : TDD::getTests())
        {
            std::cout << "--------\n"
                      << test->name() << std::endl;
            try
            {
                test->run();
            }
            catch (...)
            {
                test->setFailed("Unexpected exception thrown.");
            }
            if (test->passed())
            {
                std::cout << "Passed" << std::endl;
            }
            else
            {
                std::cout << "Failed" << std::endl
                          << test->reason() << std::endl;
            }
        }
    }
} // namespace TDD

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
