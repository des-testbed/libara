/*
 * $FU-Copyright$
 */

#include "omnetpp/MessageDispatcher.h"
#include "omnetpp/OMNeTAddress.h"
#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/OMNeTGate.h"
#include "IPv4ControlInfo.h"
#include "IPv4Address.h"
#include "ARPPacket_m.h"

#include <string>

OMNETARA_NAMESPACE_BEGIN

MessageDispatcher::MessageDispatcher(AbstractOMNeTARAClient* module, AbstractARAClient* araClient) {
    this->module = module;
    this->araClient = araClient;
}

void MessageDispatcher::setMaxTTL(int n) {
    maxTTL = n;
}

void MessageDispatcher::dispatch(cMessage* message) {
    if(isFromUpperLayer(message)) {
        handleUpperLayerMessage(message);
    }
    else if (isARPMessage(message)) {
        handleARP(message);
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
    IPv4Address sourceIP = controlInfo->getSrcAddr();
    IPv4Address destinationIP = controlInfo->getDestAddr();
    EV << "Handling upper layer message from " << sourceIP << " to " << destinationIP << ": "<< message << "\n";

    AddressPtr source = AddressPtr(new OMNeTAddress(sourceIP));
    AddressPtr destination = AddressPtr(new OMNeTAddress(destinationIP));
    AddressPtr sender = source;
    OMNeTPacket* omnetPacket = new OMNeTPacket(source, destination, sender, PacketType::DATA, araClient->getNextSequenceNumber(), maxTTL);
    omnetPacket->encapsulate(check_and_cast<cPacket*>(message));

    araClient->sendPacket(omnetPacket);
}

bool MessageDispatcher::isARPMessage(cMessage* message) {
    return dynamic_cast<ARPPacket*>(message) != NULL;
}

bool MessageDispatcher::isARAMessage(cMessage* message) {
    return dynamic_cast<OMNeTPacket*>(message) != NULL;
}

void MessageDispatcher::handleARP(cMessage* message) {
    // FIXME hasBitError() check  missing!
    delete message->removeControlInfo();

    InterfaceEntry* arrivalInterface = getSourceInterfaceFrom(message);
    ASSERT(arrivalInterface);

    IPv4RoutingDecision* routingDecision = new IPv4RoutingDecision();
    routingDecision->setInterfaceId(arrivalInterface->getInterfaceId());
    message->setControlInfo(routingDecision);

    module->send(message, "arpOut");
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
