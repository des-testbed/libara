/*
 * $FU-Copyright$
 */

#include "CppUTest/CommandLineTestRunner.h"
#include "Environment.h"
#include "testAPI/mocks/time/TimeFactoryMock.h"

using namespace ARA;

int main(int ac, char** av) {
    Environment::setClock(new TimeFactoryMock());
    return CommandLineTestRunner::RunAllTests(ac, av);
}
