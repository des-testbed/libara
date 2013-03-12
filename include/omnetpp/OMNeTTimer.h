/*
 * $FU-Copyright$
 */

#ifndef OMNET_TIMER_H_
#define OMNET_TIMER_H_

#include <omnetpp.h>

#include "Timer.h"
#include "OMNeTTimer.h"
#include "OMNeTClock.h"

namespace ARA {
namespace omnetpp {

    class OMNeTTimer : public Timer {
    public:
        OMNeTTimer(unsigned int id, OMNeTClock* clock);
        virtual ~OMNeTTimer();

        void run(unsigned long timeoutInMicroSeconds);
        void interrupt();

        void notifyTimeExpired();

    private:
        OMNeTClock* clock;
        unsigned int id;
    };
}
}

#endif 

