/*
 * $FU-Copyright$
 */

#ifndef UNIXCLOCK_H_
#define UNIXCLOCK_H_

#include "TimeFactory.h"

namespace ARA {
    class UnixClock : public TimeFactory {
        public:
            Time* makeTime();
            Timer* getNewTimer();
    };
}

#endif 
