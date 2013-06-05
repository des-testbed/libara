/*
 * $FU-Copyright$
 */

#include "TrafficMultiplexer.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(TrafficMultiplexer);

void TrafficMultiplexer::initialize(int level) {
    WATCH(nrOfSentMessages);
}

void TrafficMultiplexer::handleMessage(cMessage* message) {
    send(message, "toLowerGate");
    nrOfSentMessages++;
}

void TrafficMultiplexer::finish() {
    recordScalar("trafficSent", nrOfSentMessages);
}

OMNETARA_NAMESPACE_END
