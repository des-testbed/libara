#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/time/TimeFactoryMock.h"
#include "testAPI/mocks/time/TimeMock.h"
#include "Time.h"

using namespace ARA;

TEST_GROUP(TimeFactoryMockTest) {};

TEST(TimeFactoryMockTest, makeTime) {
    TimeFactoryMock timeFactory = TimeFactoryMock();
    Time* newTime = timeFactory.makeTime();

    TimeMock* timeMockInstance = dynamic_cast<TimeMock*>(newTime);
    CHECK(timeMockInstance != NULL);

    delete newTime;
}

TEST(TimeFactoryMockTest, getNewTimer) {
    TimeFactoryMock timeFactory = TimeFactoryMock();
    Timer* newTimer = timeFactory.getNewTimer();

    TimerMock* timerMockInstance = dynamic_cast<TimerMock*>(newTimer);
    CHECK(timerMockInstance != NULL);

    delete newTimer;
}

TEST(TimeFactoryMockTest, getLastTimer) {
    TimeFactoryMock timeFactory = TimeFactoryMock();
    Timer* timer = timeFactory.getNewTimer();
    CHECK(timeFactory.getLastTimer() == timer);

    delete timer;
}
