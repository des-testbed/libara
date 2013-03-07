/*
 * $FU-Copyright$
 */

#ifndef OMNET_TIMER_H_
#define OMNET_TIMER_H_

#include <omnetpp.h>

#include "Timer.h"

namespace ARA {
namespace omnetpp {

    class OMNeTTimer : public Timer {
    public:
        void run(double timeInMilliSeconds);
    };
}
}

#endif 

