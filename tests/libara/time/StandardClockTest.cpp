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

    StandardTime* standardTimeInstance = dynamic_cast<StandardTime*>(newTime);
    CHECK(standardTimeInstance != nullptr);

    delete newTime;
}

TEST(StandardClockTest, getNewTimer) {
    StandardClock clock;
    std::shared_ptr<Timer> newTimer = clock.getNewTimer();

    CHECK(newTimer.get() != nullptr);
}

ARA_NAMESPACE_END
