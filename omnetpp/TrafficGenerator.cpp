/*
 * $FU-Copyright$
 */

#include "omnetpp/TrafficGenerator.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(TrafficGenerator);

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
    numTrafficMsgs++;
}

void TrafficGenerator::handleLowerMsg(cPacket *message) {
    numTrafficMsgsRecv++;
    delete message;
}

void TrafficGenerator::finish() {
    recordScalar("trafficSent", numTrafficMsgs);
    recordScalar("trafficReceived", numTrafficMsgsRecv);
}

OMNETARA_NAMESPACE_END
