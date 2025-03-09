#include "Test.h"

TEST
{
    std::cout << mName << std::endl;
}

int main(int argc, const char** argv) {
    for (auto * test: TDD::getTests()) {
        test->run();
    }
    return 0;
}