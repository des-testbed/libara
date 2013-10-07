/*
 * $FU-Copyright$
 */

#include "ClockMock.h"
#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/time/TimeMock.h"
#include "testAPI/mocks/time/TimerMock.h"

ARA_NAMESPACE_BEGIN

Time* ClockMock::makeTime() {
    return new TimeMock();
}

TimerPtr ClockMock::getNewTimer(char timerType, void* contextObject) {
    lastTimer = TimerMockPtr(new TimerMock(timerType, contextObject));
    //return std::dynamic_pointer_cast<Timer>(lastTimer);
    return lastTimer;
}

TimerMockPtr ClockMock::getLastTimer() {
    if(lastTimer == nullptr) {
        FAIL("Error in ClockMock: Can not get last timer: not initialized yet!");
    }
    return lastTimer;
}

ARA_NAMESPACE_END
