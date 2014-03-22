/*
 * $FU-Copyright$
 */

#ifndef DUMMY_CLOCK_H_
#define DUMMY_CLOCK_H_

#include "ARAMacros.h"
#include "ThreadPool.h"
#include "Clock.h"

#include <thread>
#include <vector>

ARA_NAMESPACE_BEGIN

class DummyClock : public Clock {
    public:
        Time* makeTime();
        TimerPtr getNewTimer(TimerType timerType=TimerType::INVALID_TIMER, void* contextObject=nullptr);
};

ARA_NAMESPACE_END

#endif 
