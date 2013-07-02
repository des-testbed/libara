/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "TestbedClock.h"
#include "TestbedTime.h"
#include "TestbedTimer.h"

using namespace ARA;

TEST_GROUP(TestbedClockTest) {};

TEST(TestbedClockTest, makeTime) {
    testbed::TestbedClock clock = testbed::TestbedClock();
    Time* newTime = clock.makeTime();

    testbed::TestbedTime* testbedTimeInstance = dynamic_cast<testbed::TestbedTime*>(newTime);
    CHECK(testbedTimeInstance != NULL);

    delete newTime;
}

TEST(TestbedClockTest, getNewTimer) {
    testbed::TestbedClock clock = testbed::TestbedClock();
    Timer* newTimer = clock.getNewTimer();

    testbed::TestbedTimer* testbedTimerInstance = dynamic_cast<testbed::TestbedTimer*>(newTimer);
    CHECK(testbedTimerInstance != NULL);

    delete newTimer;
}
