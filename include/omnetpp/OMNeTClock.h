/*
 * $FU-Copyright$
 */

#ifndef OMNET_CLOCK_H_
#define OMNET_CLOCK_H_

#include "Clock.h"
#include "omnetpp/Environment.h"
#include "omnetpp/OMNeTTimerMessage.h"

#include <omnetpp.h>

namespace ARA {
namespace omnetpp {

    class OMNeTClock : public Clock {
        public:
            OMNeTClock(Environment* environment);
            Time* makeTime();
            Timer* getNewTimer();

            void startTimer(unsigned int timerID, long timeInMilliSeconds);
            void handleTimerMessage(OMNeTTimerMessage* msg);

        private:
            Environment* environment;
            unsigned int nrOfTimers = 0;
    };
}
}
#endif
