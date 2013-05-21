/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "UnixClock.h"
#include "UnixTime.h"
#include "UnixTimer.h"

using namespace ARA;

TEST_GROUP(UnixClockTest) {};

TEST(UnixClockTest, makeTime) {
    UnixClock clock = UnixClock();
    Time* newTime = clock.makeTime();

    UnixTime* unixTimeInstance = dynamic_cast<UnixTime*>(newTime);
    CHECK(unixTimeInstance != NULL);

    delete newTime;
}

TEST(UnixClockTest, getNewTimer) {
    UnixClock clock = UnixClock();
    Timer* newTimer = clock.getNewTimer();

    UnixTimer* unixTimerInstance = dynamic_cast<UnixTimer*>(newTimer);
    CHECK(unixTimerInstance != NULL);

    delete newTimer;
}
