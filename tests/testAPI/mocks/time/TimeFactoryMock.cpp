#include "TimeFactoryMock.h"
#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/time/TimeMock.h"
#include "testAPI/mocks/time/TimerMock.h"

using namespace ARA;

Time* TimeFactoryMock::makeTime() {
    return new TimeMock();
}

Timer* TimeFactoryMock::getNewTimer() {
    lastTimer = new TimerMock();
    return lastTimer;
}

TimerMock* TimeFactoryMock::getLastTimer() {
    if(lastTimer == nullptr) {
        FAIL("Error in TimeFactoryMock: Can not get last timer: not initialized yet!");
    }
    return lastTimer;
}
