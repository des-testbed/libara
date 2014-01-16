/*
 * $FU-Copyright$
 */

#ifndef STANDARD_CLOCK_H_
#define STANDARD_CLOCK_H_

#include "ARAMacros.h"
#include "Clock.h"

ARA_NAMESPACE_BEGIN

class StandardClock : public Clock {
    public:
        Time* makeTime();
        Timer* getNewTimer(TimerType timerType=TimerType::INVALID_TIMER, void* contextObject=nullptr);
};

ARA_NAMESPACE_END

#endif 
