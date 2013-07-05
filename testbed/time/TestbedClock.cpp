#include "TestbedClock.h"

TESTBED_NAMESPACE_BEGIN

Time* TestbedClock::makeTime(){
    return new TestbedTime();
}

Timer* TestbedClock::getNewTimer(char timerType, void* contextObject){
    return new TestbedTimer(timerType, contextObject);
}

TESTBED_NAMESPACE_END
