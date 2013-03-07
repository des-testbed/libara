#include "ClockMock.h"
#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/time/TimeMock.h"
#include "testAPI/mocks/time/TimerMock.h"

using namespace ARA;

Time* ClockMock::makeTime() {
    return new TimeMock();
}

Timer* ClockMock::getNewTimer() {
    lastTimer = new TimerMock();
    return lastTimer;
}

TimerMock* ClockMock::getLastTimer() {
    if(lastTimer == nullptr) {
        FAIL("Error in ClockMock: Can not get last timer: not initialized yet!");
    }
    return lastTimer;
}
