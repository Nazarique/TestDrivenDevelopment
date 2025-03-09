#include "../Test.h"
#include <iostream>
#include <fstream>

int main(int argc, const char **argv)
{

    TDD::runTests();
    /*
    std::ofstream file("output.txt");

    if (file.is_open())
    {
        TDD::setOutStream(file);
        TDD::runTests();
    } */
    return 0;
}