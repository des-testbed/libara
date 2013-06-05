/*
 * $FU-Copyright$
 */

#include "TrafficSink.h"
#include "TrafficPacket_m.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(TrafficSink);

void TrafficSink::initialize(int level) {
    if(level == 0) {
        delayVector.setName("delay");
        WATCH(nrOfReceivedMessages);
    }
}

void TrafficSink::handleMessage(cMessage* message) {
    TrafficPacket* packet = check_and_cast<TrafficPacket*>(message);

    SimTime currentTime = simTime();
    SimTime delay = currentTime - packet->getCreationTime();

    delayVector.record(delay);
    nrOfReceivedMessages++;

    delete message;
}

void TrafficSink::finish() {
    recordScalar("trafficReceived", nrOfReceivedMessages);
}

OMNETARA_NAMESPACE_END
