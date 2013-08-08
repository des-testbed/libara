/*
 * $FU-Copyright$
 */

#ifndef OMNET_CLOCK_H_
#define OMNET_CLOCK_H_

#include "Clock.h"
#include "omnetpp/OMNeTTimerMessage.h"

#include <omnetpp.h>
#include <unordered_map>
#include <deque>

namespace ARA {
namespace omnetpp {

    class OMNeTTimer;
    typedef std::shared_ptr<OMNeTTimer> OMNeTTimerPtr;

    class OMNeTClock : public Clock, public cSimpleModule {
        public:
            Time* makeTime();
            TimerPtr getNewTimer(char timerType=0, void* contextObject=nullptr);

            void startTimer(unsigned int timerID, unsigned long timeoutInMicroSeconds);
            void stopTimer(unsigned int timerID);
            void timerHasBeenDeleted(unsigned int timerID);

            void initialize();
            void handleMessage(cMessage *msg);
            void finish();

        private:
            bool isInitialized = false;
            unsigned int timerIDCounter = 0;
            std::unordered_map<unsigned int, OMNeTTimerPtr> runningTimers;
            std::unordered_map<unsigned int, cMessage*> pendingSelfMessages;

            void cancelTimerEvents(unsigned int timerID);
    };
}
}
#endif
