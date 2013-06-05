/*
 * $FU-Copyright$
 */

#ifndef TRAFFIC_SINK_H_
#define TRAFFIC_SINK_H_

#include "OMNeTARAMacros.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * This class just receives and consumes traffic.
 * It will count some of the metrics like delay and number of received packets as well.
 */
class TrafficSink : public cSimpleModule {
    public:
        virtual void initialize(int level);
        void handleMessage(cMessage* message);
        virtual void finish();

    private:
        int nrOfReceivedMessages = 0;
        cOutVector delayVector;
};

OMNETARA_NAMESPACE_END
#endif
