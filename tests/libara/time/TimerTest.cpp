#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/time/TimerMock.h"
#include "testAPI/mocks/time/TimeoutEventListenerMock.h"

using namespace ARA;

TEST_GROUP(TimerTest) {};

TEST(TimerTest, addTimeoutListener) {
    TimerMock timer = TimerMock();
    TimeoutEventListenerMock listener1 = TimeoutEventListenerMock();
    TimeoutEventListenerMock listener2 = TimeoutEventListenerMock();
    TimeoutEventListenerMock listener3 = TimeoutEventListenerMock();

    timer.addTimeoutListener(&listener1);
    timer.addTimeoutListener(&listener2);
    timer.addTimeoutListener(&listener3);

    CHECK_FALSE(listener1.hasBeenNotified());
    CHECK_FALSE(listener2.hasBeenNotified());
    CHECK_FALSE(listener3.hasBeenNotified());

    timer.expire();

    CHECK_TRUE(listener1.hasBeenNotified());
    CHECK_TRUE(listener2.hasBeenNotified());
    CHECK_TRUE(listener3.hasBeenNotified());
}
