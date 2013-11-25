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
        Timer* getNewTimer(char timerType=-1, void* contextObject=nullptr);
};

ARA_NAMESPACE_END

#endif 
