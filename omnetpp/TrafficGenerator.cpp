/*
 * $FU-Copyright$
 */

#include "omnetpp/TrafficGenerator.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(TrafficGenerator);

void TrafficGenerator::initialize(int level) {
    TrafGen::initialize(level);
    if(level == 0) {
        WATCH(nrOfSentMessages);
        WATCH(nrOfReceivedMessages);
    }
}

void TrafficGenerator::SendTraf(cPacket *message, const char *destination) {
    IPv4ControlInfo* controlInfo = new  IPv4ControlInfo();
    IPv4Address sourceAddress("192.168.0.1"); //TODO get this from the interface table or via a configuration parameter
    IPv4Address destinationAddress(destination);
    controlInfo->setSrcAddr(sourceAddress);
    controlInfo->setDestAddr(destinationAddress);

    IPv4Datagram* datagram = new IPv4Datagram("Traffic");
    datagram->encapsulate(message);
    datagram->setControlInfo(controlInfo);
    send(datagram, "lowergate$o");
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
