/*
 * $FU-Copyright$
 */

#ifndef TESTBED_CLOCK_H_
#define TESTBED_CLOCK_H_

#include "Time.h"
#include "Timer.h"
#include "Clock.h"

#include "Testbed.h"
#include "TestbedTime.h"
#include "TestbedTimer.h"

TESTBED_NAMESPACE_BEGIN

class TestbedClock : public Clock {
    public:
        Time* makeTime();
        Timer* getNewTimer(char timerType=-1, void* contextObject=nullptr);
};

TESTBED_NAMESPACE_END

#endif 
