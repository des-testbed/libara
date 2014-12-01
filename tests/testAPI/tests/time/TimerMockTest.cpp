/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/libara/time/TimerMock.h"
#include "Exception.h"

using namespace ARA;

TEST_GROUP(TimerMockTest) {};

TEST(TimerMockTest, expire) {
    TimerMock timer(TimerType::INVALID_TIMER);
    timer.run(123);
    timer.expire();

    CHECK(timer.hasExpired());
}

TEST(TimerMockTest, hasBeenInterrupted) {
    TimerMock timer(TimerType::INVALID_TIMER);
    CHECK_FALSE(timer.hasBeenInterrupted());

    timer.run(123);
    CHECK_FALSE(timer.hasBeenInterrupted());

    timer.interrupt();
    CHECK_TRUE(timer.hasBeenInterrupted());
}
