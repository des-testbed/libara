#include "TestbedClock.h"

TESTBED_NAMESPACE_BEGIN

Time* TestbedClock::makeTime(){
    return new TestbedTime();
}

Timer* TestbedClock::getNewTimer(){
    return new TestbedTimer();
}

TESTBED_NAMESPACE_END
