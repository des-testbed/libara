/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/time/ClockMock.h"
#include "testAPI/mocks/time/TimeMock.h"
#include "Time.h"

using namespace ARA;

TEST_GROUP(ClockMockTest) {};

TEST(ClockMockTest, makeTime) {
    ClockMock clock = ClockMock();
    Time* newTime = clock.makeTime();

    TimeMock* timeMockInstance = dynamic_cast<TimeMock*>(newTime);
    CHECK(timeMockInstance != NULL);

    delete newTime;
}

TEST(ClockMockTest, getNewTimer) {
    ClockMock clock = ClockMock();
    TimerPtr newTimer = clock.getNewTimer();

    TimerMockPtr timerMockInstance = std::dynamic_pointer_cast<TimerMock>(newTimer);
    CHECK(timerMockInstance != NULL);
}

TEST(ClockMockTest, getLastTimer) {
    ClockMock clock = ClockMock();
    TimerPtr timer = clock.getNewTimer();
    CHECK(clock.getLastTimer() == timer);
}
