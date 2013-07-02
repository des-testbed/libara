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
    TestbedClock clock = TestbedClock();
    Time* newTime = clock.makeTime();

    TestbedTime* testbedTimeInstance = dynamic_cast<TestbedTime*>(newTime);
    CHECK(testbedTimeInstance != NULL);

    delete newTime;
}

TEST(TestbedClockTest, getNewTimer) {
    TestbedClock clock = TestbedClock();
    Timer* newTimer = clock.getNewTimer();

    TestbedTimer* testbedTimerInstance = dynamic_cast<TestbedTimer*>(newTimer);
    CHECK(testbedTimerInstance != NULL);

    delete newTimer;
}
