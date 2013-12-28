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
    StandardClock clock;
    Time* newTime = clock.makeTime();

    StandardTime* time = dynamic_cast<StandardTime*>(newTime);
    CHECK(time != nullptr);

    delete newTime;
}

TEST(StandardClockTest, getNewTimer) {
    StandardClock clock;

    StandardTimer* timer = dynamic_cast<StandardTimer*>(clock.getNewTimer());
    CHECK(timer != nullptr);

    delete timer;
}

ARA_NAMESPACE_END
