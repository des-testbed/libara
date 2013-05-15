/*
 * $FU-Copyright$
 */

#ifndef OMNET_TIMER_MSG_H_
#define OMNET_TIMER_MSG_H_

#include <omnetpp.h>

namespace ARA {
namespace omnetpp {

    class OMNeTTimerMessage : public cMessage {
    public:
        OMNeTTimerMessage(unsigned int timerID);
        unsigned int getTimerID();

    private:
        unsigned int timerID;
    };
}
}

#endif 

