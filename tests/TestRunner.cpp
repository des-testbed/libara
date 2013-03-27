/*
 * $FU-Copyright$
 */

#include "CppUTest/CommandLineTestRunner.h"
#include "Environment.h"
#include "Exception.h"
#include "testAPI/mocks/time/ClockMock.h"
#include <iostream>

using namespace ARA;

int main(int ac, char** av) {
    try {
        Environment::setClock(new ClockMock());
        return CommandLineTestRunner::RunAllTests(ac, av);
    } catch(ARA::Exception& exception) {
        std::cout << "\nERROR: Tests failed with ARA::Exception: " << exception.getMessage() << "\n\n";
        return 1;
    }
}
