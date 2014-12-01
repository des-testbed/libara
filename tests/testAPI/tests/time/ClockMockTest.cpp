/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/libara/time/ClockMock.h"
#include "testAPI/mocks/libara/time/TimeMock.h"
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
    std::shared_ptr<Timer> newTimer = clock.getNewTimer(TimerType::INVALID_TIMER);

    std::shared_ptr<TimerMock> timerMockInstance = std::dynamic_pointer_cast<TimerMock>(newTimer);
    CHECK(timerMockInstance != NULL);
}

TEST(ClockMockTest, getLastTimer) {
    ClockMock clock = ClockMock();
    std::shared_ptr<Timer> timer = clock.getNewTimer(TimerType::INVALID_TIMER);
    CHECK((clock.getLastTimer()).lock() == timer);
}
