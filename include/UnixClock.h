/*
 * $FU-Copyright$
 */

#ifndef UNIXCLOCK_H_
#define UNIXCLOCK_H_

#include "ARAMacros.h"
#include "Clock.h"

ARA_NAMESPACE_BEGIN

class UnixClock : public Clock {
    public:
        virtual Time* makeTime();
        virtual Timer* getNewTimer(char timerType=-1, void* contextObject=nullptr);
};

ARA_NAMESPACE_END

#endif 
