/*
 * $FU-Copyright$
 */

#include "ClockMock.h"
#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/time/TimeMock.h"
#include "testAPI/mocks/time/TimerMock.h"

using namespace ARA;

Time* ClockMock::makeTime() {
    return new TimeMock();
}

std::shared_ptr<Timer> ClockMock::getNewTimer(TimerType timerType, void* contextObject) {
    std::shared_ptr<TimerMock> timer = std::make_shared<TimerMock>(timerType, contextObject);
    lastTimer = timer;
    return timer;
}

std::weak_ptr<TimerMock> ClockMock::getLastTimer() {
    return lastTimer;
}
