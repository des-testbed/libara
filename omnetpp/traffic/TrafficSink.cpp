/*
 * $FU-Copyright$
 */

#include "TrafficSink.h"
#include "TrafficPacket_m.h"
#include "TrafficControllInfo.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(TrafficSink);

void TrafficSink::initialize(int level) {
    if(level == 0) {
        delayVector.setName("delay");
        hopCountVector.setName("hopCount");
        routeEnergyOutVector.setName("routeEnergy");
        WATCH(nrOfReceivedMessages);
    }
}

void TrafficSink::handleMessage(cMessage* message) {
    TrafficPacket* packet = check_and_cast<TrafficPacket*>(message);

    SimTime currentTime = simTime();
    SimTime delay = currentTime - packet->getCreationTime();
    timeOfLastReceivedPacket = currentTime;

    delayVector.record(delay);
    nrOfReceivedMessages++;

    TrafficControlInfo* controlInfo = check_and_cast<TrafficControlInfo*>(packet->getControlInfo());
    hopCountVector.record(controlInfo->getHopCount());

    routeEnergyOutVector.record(packet->getRouteEnergy());

    delete message;
}

void TrafficSink::finish() {
    recordScalar("trafficReceived", nrOfReceivedMessages);
    recordScalar("timeOfLastReceivedPacket", timeOfLastReceivedPacket);
}

OMNETARA_NAMESPACE_END
