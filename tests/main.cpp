#include "../Test.h"
#include <iostream>

int main(int argc, const char **argv)
{

    TDD::setOutStream(std::cout);
    TDD::runTests();
    return 0;
}