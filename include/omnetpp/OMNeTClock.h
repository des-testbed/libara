/*
 * $FU-Copyright$
 */

#ifndef OMNET_CLOCK_H_
#define OMNET_CLOCK_H_

#include "Clock.h"

namespace ARA {
namespace omnetpp {

    class OMNeTClock : public Clock {
        public:
            Time* makeTime();
            Timer* getNewTimer();
    };
}
}
#endif
