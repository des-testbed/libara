/*
 * $FU-Copyright$
 */

#include "CppUTest/CommandLineTestRunner.h"
#include "Environment.h"
#include "testAPI/mocks/time/ClockMock.h"

using namespace ARA;

int main(int ac, char** av) {
    Environment::setClock(new ClockMock());
    return CommandLineTestRunner::RunAllTests(ac, av);
}
