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

void TrafficGenerator::sendTraffic(cPacket* originalTrafGenMessage, const char* destination) {
    TrafficPacket* packet = new TrafficPacket("Traffic");
    SimTime now = simTime();
    packet->setCreationTime(now);
    long packetSizeInBytes = PacketSize();
    packet->setByteLength(packetSizeInBytes);

    TrafficControlInfo* controlInfo = new TrafficControlInfo();
    controlInfo->setDestination(destination);
    packet->setControlInfo(controlInfo);

    send(packet, "toLowerGate");
    nrOfSentMessages++;

    delete originalTrafGenMessage;
}

OMNETARA_NAMESPACE_END
