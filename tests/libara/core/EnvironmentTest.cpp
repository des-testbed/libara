/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "Environment.h"
#include "TimeFactory.h"
#include "testAPI/mocks/time/TimeFactoryMock.h"

using namespace ARA;

TEST_GROUP(EnvironmentTest) {};

/**
 * We test if we can retrieve some instance of Time and make sure it can be used.
 */
TEST(EnvironmentTest, getClock) {
    TimeFactory* clock = Environment::getClock();
    Time* time = clock->makeTime();
    time->setToCurrentTime();
    delete time;
}

//FIXME I dont get why this causes a memory leak...
IGNORE_TEST(EnvironmentTest, setClock) {
    TimeFactory* newClock = new TimeFactoryMock();
    Environment::setClock(newClock);

    TimeFactory* clock = Environment::getClock();
    CHECK(clock == newClock);
}
