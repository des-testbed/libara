/*
 * $FU-Copyright$
 */

#ifndef UNIXCLOCK_H_
#define UNIXCLOCK_H_

#include "Clock.h"

namespace ARA {
    class UnixClock : public Clock {
        public:
            Time* makeTime();
            Timer* getNewTimer();
    };
}

#endif 
