#include <string_view>
#include <iostream>

class Test {

    public: 
        Test (std::string_view name) 
        : mName(name), mResult(true)
        {}
        void operator () ()
        {
            std::cout << mName << std::endl;
        }
        
    private: 
        std::string mName;
        bool mResult;
};

Test test("testCanBeCreated");

int main(int argc, const char** argv) {

    test();
    return 0;
}