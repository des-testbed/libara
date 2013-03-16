/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "Environment.h"
#include "Clock.h"
#include "testAPI/mocks/time/ClockMock.h"

using namespace ARA;

TEST_GROUP(EnvironmentTest) {};

/**
 * We test if we can retrieve some instance of Time and make sure it can be used.
 */
TEST(EnvironmentTest, getClock) {
    Clock* clock = Environment::getClock();
    Time* time = clock->makeTime();
    time->setToCurrentTime();
    delete time;
}
