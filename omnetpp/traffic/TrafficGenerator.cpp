/*
 * $FU-Copyright$
 */

#include "TrafficGenerator.h"
#include "TrafficControllInfo.h"
#include "TrafficPacket_m.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(TrafficGenerator);

void TrafficGenerator::initialize(int level) {
    TrafGen::initialize(level);
    if(level == 0) {
        nrOfPacketsToSend = par("nrOfPackets").longValue();
        delayVector.setName("delay");
        WATCH(nrOfSentMessages);
        WATCH(nrOfReceivedMessages);
    }
}

void TrafficGenerator::SendTraf(cPacket* message, const char* destination) {
    if(nrOfPacketsToSend > 0) {
        if(nrOfSentMessages < nrOfPacketsToSend) {
            sendTraffic(message, destination);
        }
        else {
            delete message;
        }
    }
    else {
        sendTraffic(message, destination);
    }
}

void TrafficGenerator::sendTraffic(cPacket* message, const char* destination) {
    // TODO we may no longer need the inheritance from the stupid TrafGen if we don#t even use its message objects anymore
    delete message;

    TrafficPacket* packet = new TrafficPacket("Traffic");
    SimTime now = simTime();
    packet->setCreationTime(now);

    TrafficControlInfo* controlInfo = new TrafficControlInfo(destination);
    packet->setControlInfo(controlInfo);

    send(packet, "lowergate$o");
    nrOfSentMessages++;
}

void TrafficGenerator::handleLowerMsg(cPacket* message) {
    TrafficPacket* packet = check_and_cast<TrafficPacket*>(message);

    SimTime currentTime = simTime();
    SimTime delay = currentTime - packet->getCreationTime();

    delayVector.record(delay);

    nrOfReceivedMessages++;
    delete message;
}

void TrafficGenerator::finish() {
    recordScalar("trafficSent", nrOfSentMessages);
    recordScalar("trafficReceived", nrOfReceivedMessages);
}

OMNETARA_NAMESPACE_END
