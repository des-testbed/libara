/*
 * $FU-Copyright$
 */

#include "omnetpp/TrafficGenerator.h"
#include "IInterfaceTable.h"
#include "ModuleAccess.h"
#include "IPv4InterfaceData.h"
#include "IPv4Address.h"

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
    IPv4ControlInfo* controlInfo = new  IPv4ControlInfo();
    IPv4Address sourceAddress = getLocalAddress();
    IPv4Address destinationAddress(destination);
    controlInfo->setSrcAddr(sourceAddress);
    controlInfo->setDestAddr(destinationAddress);

    IPv4Datagram* datagram = new IPv4Datagram("Traffic");
    datagram->encapsulate(message);
    datagram->setControlInfo(controlInfo);
    send(datagram, "lowergate$o");
    nrOfSentMessages++;
}

IPv4Address TrafficGenerator::getLocalAddress() {
    //TODO this does currently only support one interface
    IInterfaceTable* interfaceTable = ModuleAccess<IInterfaceTable>("interfaceTable").get(this);
    InterfaceEntry* interface = nullptr;
    int nrOfInterfaces = interfaceTable->getNumInterfaces();
    for (int k=0; k < nrOfInterfaces; k++) {
        InterfaceEntry* currentInterfaceEntry = interfaceTable->getInterface(k);
        if(currentInterfaceEntry->isLoopback() == false) {
            if(interface != nullptr) {
                throw cRuntimeError("The TrafficGenerator does currently only support one single interface per node");
            }
            else {
                interface = currentInterfaceEntry;
            }
        }
    }

    if(interface == nullptr) {
        throw cRuntimeError("The TrafficGenerator could not determine the nodes interface");
    }

    return interface->ipv4Data()->getIPAddress();
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
