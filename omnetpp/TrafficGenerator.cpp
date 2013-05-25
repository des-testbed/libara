/*
 * $FU-Copyright$
 */

#include "omnetpp/TrafficGenerator.h"
#include "omnetpp/TrafficControllInfo.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(TrafficGenerator);

void TrafficGenerator::initialize(int level) {
    TrafGen::initialize(level);
    if(level == 0) {
        nrOfPacketsToSend = par("nrOfPackets").longValue();
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
    TrafficControlInfo* controlInfo = new TrafficControlInfo(destination);
    message->setControlInfo(controlInfo);
    send(message, "lowergate$o");
    nrOfSentMessages++;
}

void TrafficGenerator::handleLowerMsg(cPacket *message) {
    nrOfReceivedMessages++;
    delete message;
}

void TrafficGenerator::finish() {
    recordScalar("trafficSent", nrOfSentMessages);
    recordScalar("trafficReceived", nrOfReceivedMessages);
}

OMNETARA_NAMESPACE_END
