/*
 * $FU-Copyright$
 */

#include "omnetpp/MessageDispatcher.h"
#include "omnetpp/OMNeTAddress.h"
#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/OMNeTGate.h"
#include "IPv4ControlInfo.h"
#include "IPv4Address.h"
#include "MACAddress.h"

#include <string>

OMNETARA_NAMESPACE_BEGIN

MessageDispatcher::MessageDispatcher(AbstractOMNeTARAClient* module, AbstractARAClient* araClient) {
    this->module = module;
    this->araClient = araClient;
}

void MessageDispatcher::setPacketFactory(PacketFactory* factory) {
    packetFactory = factory;
}

void MessageDispatcher::dispatch(cMessage* message) {
    if(isFromUpperLayer(message)) {
        handleUpperLayerMessage(message);
    }
    else {
        handleARA(message);
    }
}

bool MessageDispatcher::isFromUpperLayer(cMessage* message) {
    std::string nameOfUpperLayergate = "upperLayerGate$i";
    std::string gateName = std::string(message->getArrivalGate()->getName());
    return gateName.length() <= nameOfUpperLayergate.length()
        && std::equal(gateName.begin(), gateName.end(), nameOfUpperLayergate.begin());
}

void MessageDispatcher::handleUpperLayerMessage(cMessage* message) {
    IPv4ControlInfo* controlInfo = (IPv4ControlInfo*)message->getControlInfo();
    IPv4Address destinationIP = controlInfo->getDestAddr();
    AddressPtr destination = AddressPtr(new OMNeTAddress(destinationIP));

    IPv4Address sourceIP = controlInfo->getSrcAddr();
    AddressPtr source = AddressPtr(new OMNeTAddress(sourceIP));;
    AddressPtr sender = source;

    OMNeTPacket* omnetPacket = packetFactory->createOMNetPacket(source, destination, sender, PacketType::DATA, araClient->getNextSequenceNumber());
    omnetPacket->encapsulate(check_and_cast<cPacket*>(message));

    araClient->sendPacket(omnetPacket);
}

InterfaceEntry* MessageDispatcher::getSourceInterfaceFrom(cMessage* message) {
    cGate* arrivalGate = message->getArrivalGate();
    if(arrivalGate != NULL) {
        return module->getInterfaceTable()->getInterfaceByNetworkLayerGateIndex(arrivalGate->getIndex());
    }
    else {
        return NULL;
    }
}

void MessageDispatcher::handleARA(cMessage* message) {
    OMNeTPacket* omnetPacket = check_and_cast<OMNeTPacket*>(message);
    ASSERT(omnetPacket->getTTL() > 0);
    OMNeTGate* arrivalGate = (OMNeTGate*) araClient->getNetworkInterface(message->getArrivalGate()->getIndex());
    arrivalGate->receive(omnetPacket);
}

OMNETARA_NAMESPACE_END
