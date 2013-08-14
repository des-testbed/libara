/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"

#include "CppUTest/TestRegistry.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "EnvironmentPlugin.h"
#include "Exception.h"
#include "testAPI/mocks/time/ClockMock.h"
#include <iostream>

using namespace ARA;

int main(int ac, char** av) {
    try {
        TestRegistry* r = TestRegistry::getCurrentRegistry();
        EnvironmentPlugin environmentPlugin("EnvironmentPlugin");
        r->installPlugin(&environmentPlugin);
        return CommandLineTestRunner::RunAllTests(ac, av);
    } catch(ARA::Exception& exception) {
        std::cout << "\nERROR: Tests failed with ARA::Exception: " << exception.getMessage() << "\n\n";
        return 1;
    } catch(const char* exceptionMessage) {
        std::cout << "\nERROR: Tests failed with const char* exception: " << exceptionMessage << "\n\n";
        return 1;
    }
}
