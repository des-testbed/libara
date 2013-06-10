/*
 * $FU-Copyright$
 */

#ifndef TRAFFIC_MULTIPLEXER_H_
#define TRAFFIC_MULTIPLEXER_H_

#include "OMNeTARAMacros.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * The class provides a traffic generator for the ARA/EARA routing
 * protocols. It is based on the INETMANET class TrafGen.
 */
class TrafficMultiplexer : public cSimpleModule {
    public:
        virtual void initialize(int level);
        void handleMessage(cMessage* message);
        virtual void finish();

    private:
        int nrOfSentMessages = 0;
};

OMNETARA_NAMESPACE_END
#endif
