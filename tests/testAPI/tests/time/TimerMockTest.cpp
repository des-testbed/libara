/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/time/TimerMock.h"
#include "Exception.h"

using namespace ARA;

TEST_GROUP(TimerMockTest) {};

TEST(TimerMockTest, expire) {
    TimerMock timer = TimerMock();
    timer.run(123);
    timer.expire();

    CHECK(timer.hasExpired());
}

TEST(TimerMockTest, hasBeenInterrupted) {
    TimerMock timer = TimerMock();
    CHECK_FALSE(timer.hasBeenInterrupted());

    timer.run(123);
    CHECK_FALSE(timer.hasBeenInterrupted());

    timer.interrupt();
    CHECK_TRUE(timer.hasBeenInterrupted());
}
