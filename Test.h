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
} // namespace TDD

#define TEST class Test { \
    public: \
        Test (std::string_view name) \
        : mName(name), mResult(true) \
        {} \
        void operator() (); \
    private: \
        std::string mName; \
        bool mResult; \
}; \
Test test("testCanBeCreated"); \
void Test::operator() () 

#endif // !TDD_TEST_H"