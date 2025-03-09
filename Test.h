#ifndef TEST_H
#define TEST_H

#include <string_view>
#include <iostream>

#define TEST class Test { \
    public: \
        Test (std::string_view name) \
        : mName(name), mResult(true) \
        {} \
        void operator () (); \
    private: \
        std::string mName; \
        bool mResult; \
}; \
Test test("testCanBeCreated"); \
void Test::operator() () 

#endif // !TEST_H