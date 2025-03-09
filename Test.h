#ifndef TDD_TEST_H
#define TDD_TEST_H

#include <vector>
#include <string_view>
#include <iostream>

namespace TDD
{
    class TestInterface {
        public: 
            virtual ~TestInterface() = default;
            virtual void run() = 0;
    };

    std::vector<TestInterface *> & getTests() {
        static std::vector<TestInterface *> tests;
        return tests;
    }

    void runTests() {
        for (auto * test: TDD::getTests()) {
            test->run();
        }
    }
} // namespace TDD

#define TEST \
class Test : public TDD::TestInterface { \
    public: \
        Test (std::string_view name) \
        : mName(name), mResult(true) \
        { \
            TDD::getTests().push_back(this); \
        } /* The Test constructor register itself. */ \
        void run() override; \
    private: \
        std::string mName; \
        bool mResult; \
}; \
Test test("testCanBeCreated"); \
void Test::run() 
#endif // TDD_TEST_H"