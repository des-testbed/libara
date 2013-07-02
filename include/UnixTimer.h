/*
 * $FU-Copyright$
 */

#ifndef UNIXTIMER_H_
#define UNIXTIMER_H_

#include "ARAMacros.h"
#include "Timer.h"

ARA_NAMESPACE_BEGIN

/**
 * FIXME implement this class
 */
class UnixTimer : public Timer {
    public:
        UnixTimer(char type) : Timer(type) {};
        void run(unsigned long timeoutInMicroSeconds);
        void interrupt();
};

ARA_NAMESPACE_END

#endif 

