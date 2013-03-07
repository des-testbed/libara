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
    Timer* newTimer = clock.getNewTimer();

    TimerMock* timerMockInstance = dynamic_cast<TimerMock*>(newTimer);
    CHECK(timerMockInstance != NULL);

    delete newTimer;
}

TEST(ClockMockTest, getLastTimer) {
    ClockMock clock = ClockMock();
    Timer* timer = clock.getNewTimer();
    CHECK(clock.getLastTimer() == timer);

    delete timer;
}
