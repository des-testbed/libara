/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/libara/time/TimerMock.h"
#include "testAPI/mocks/libara/time/TimeoutEventListenerMock.h"

using namespace ARA;

TEST_GROUP(TimerTest) {};

TEST(TimerTest, addTimeoutListener) {
    std::shared_ptr<TimerMock> timer = std::make_shared<TimerMock>(TimerType::INVALID_TIMER);
    TimeoutEventListenerMock listener1;
    TimeoutEventListenerMock listener2;
    TimeoutEventListenerMock listener3;

    timer->addTimeoutListener(&listener1);
    timer->addTimeoutListener(&listener2);
    timer->addTimeoutListener(&listener3);

    CHECK_FALSE(listener1.hasBeenNotified());
    CHECK_FALSE(listener2.hasBeenNotified());
    CHECK_FALSE(listener3.hasBeenNotified());

    timer->expire();

    CHECK_TRUE(listener1.hasBeenNotified());
    CHECK_TRUE(listener2.hasBeenNotified());
    CHECK_TRUE(listener3.hasBeenNotified());
}

TEST(TimerTest, defaultType) {
    TimerMock timer(TimerType::INVALID_TIMER);
    BYTES_EQUAL(6, timer.getType());
}

TEST(TimerTest, defaultContextObject) {
    TimerMock timer(TimerType::INVALID_TIMER);
    CHECK(timer.getContextObject() == nullptr);
}

TEST(TimerTest, getType) {
    TimerMock timer = TimerMock(TimerType::ROUTE_DISCOVERY_DELAY_TIMER);
    BYTES_EQUAL(TimerType::ROUTE_DISCOVERY_DELAY_TIMER, timer.getType());
}

TEST(TimerTest, getContextObject) {
    const char* contextObject = "Hello World";
    TimerMock timer = TimerMock(TimerType::PANTS_TIMER, (void*) contextObject);
    CHECK_EQUAL(contextObject, timer.getContextObject());
}

TEST(TimerTest, setContextObject) {
    TimerMock timer(TimerType::INVALID_TIMER);
    CHECK(timer.getContextObject() == nullptr);

    const char* contextObject = "Hello World";
    timer.setContextObject((void*)contextObject);
    CHECK_EQUAL(contextObject, timer.getContextObject());
}
