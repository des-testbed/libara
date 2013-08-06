/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "StandardClock.h"
#include "StandardTime.h"
#include "StandardTimer.h"

ARA_NAMESPACE_BEGIN

TEST_GROUP(StandardClockTest) {};

TEST(StandardClockTest, makeTime) {
    StandardClock clock = StandardClock();
    Time* newTime = clock.makeTime();

    StandardTime* standardTimeInstance = dynamic_cast<StandardTime*>(newTime);
    CHECK(standardTimeInstance != NULL);

    delete newTime;
}

TEST(StandardClockTest, getNewTimer) {
    StandardClock clock = StandardClock();
    Timer* newTimer = clock.getNewTimer();

    StandardTimer* standardTimerInstance = dynamic_cast<StandardTimer*>(newTimer);
    CHECK(standardTimerInstance != NULL);

    delete newTimer;
}

ARA_NAMESPACE_END
