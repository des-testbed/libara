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
        OMNeTTimer(unsigned int id, OMNeTClock* clock, TimerType type=TimerType::INVALID_TIMER, void* contextObject=nullptr);
        virtual ~OMNeTTimer();

        void run(unsigned long timeoutInMicroSeconds);
        void interrupt();

        void notifyTimeExpired();

        bool equals(const Timer* otherTimer) const;
        bool equals(const std::shared_ptr<Timer> otherTimer) const;

        size_t getHashValue() const;

    private:
        OMNeTClock* clock;
        unsigned int id;
    };
}
}

#endif 

