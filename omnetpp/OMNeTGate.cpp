/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTGate.h"
#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/OMNeTAddress.h"
#include "Environment.h"
#include "IInterfaceTable.h"
#include "IPv4InterfaceData.h"
#include "IPvXAddressResolver.h"
#include "IPv4ControlInfo.h"
#include "IPv4Datagram.h"

using namespace std;

OMNETARA_NAMESPACE_BEGIN

OMNeTGate::OMNeTGate(AbstractOMNeTARAClient* module, AbstractARAClient* araClient, cGate* gateToARP, InterfaceEntry* interfaceEntry, double broadCastDelay, double uniCastDelay) : AbstractNetworkInterface(araClient) {
    this->omnetARAModule = module;
    this->gateToARP = gateToARP;
    this->broadCastDelay = broadCastDelay;
    this->uniCastDelay = uniCastDelay;

    IPv4Address localAddress = IPvXAddressResolver().getAddressFrom(interfaceEntry, IPvXAddressResolver::ADDR_IPv4).get4();
    IPv4Address netmask = interfaceEntry->ipv4Data()->getNetmask();
    IPv4Address networkAddress = localAddress.doAnd(netmask);
    IPv4Address broadcastAddress = networkAddress.getBroadcastAddress(netmask);

    this->localAddress = shared_ptr<Address>(new OMNeTAddress(localAddress));
    this->broadcastAddress = shared_ptr<Address>(new OMNeTAddress(broadcastAddress));
    this->interfaceID = interfaceEntry->getInterfaceId();
    this->packetFactory = araClient->getPacketFactory();
}

void OMNeTGate::send(const Packet* packet, shared_ptr<Address> recipient) {
    send(packet, recipient, uniCastDelay);
}

void OMNeTGate::send(const Packet* packet, shared_ptr<Address> recipient, double sendDelay) {
    // TODO somehow remove this ugly casting stuff
    OMNeTPacket* originalPacket = (OMNeTPacket*) packet;
    OMNeTPacket* omnetPacket = (OMNeTPacket*) packetFactory->makeClone(originalPacket);
    OMNeTAddressPtr nextHopAddress = getNextHopAddress(recipient);

    // Get the encapsulated packet (if any)
    if(originalPacket->getEncapsulatedPacket()) {
        cPacket* encapsulatedPacket = originalPacket->decapsulate();
        omnetPacket->encapsulate(encapsulatedPacket);
    }

    IPv4RoutingDecision* controlInfo = new IPv4RoutingDecision();
    controlInfo->setNextHopAddr(*(nextHopAddress.get()));
    controlInfo->setInterfaceId(interfaceID);
    omnetPacket->setControlInfo(controlInfo);

    // we might have switched the context from the OMNeTTimer
    omnetARAModule->takeAndSend(omnetPacket, gateToARP, sendDelay);
}

OMNeTAddressPtr OMNeTGate::getNextHopAddress(shared_ptr<Address> recipient) {
    shared_ptr<OMNeTAddress> nextHopAddress (dynamic_pointer_cast<OMNeTAddress>(recipient));
    if(nextHopAddress == nullptr) {
        throw cRuntimeError("Error in OMNeTGate: Can only send packets to OMNeTAddress recipients");
    }
    return nextHopAddress;
}

void OMNeTGate::broadcast(const Packet* packet) {
    send(packet, broadcastAddress, broadCastDelay);
}

bool OMNeTGate::equals(NetworkInterface* otherInterface) {
    OMNeTGate* otherOMNeTInterface = dynamic_cast<OMNeTGate*>(otherInterface);
    if(otherOMNeTInterface == NULL) {
        return false;
    }
    else {
        return strcmp(omnetARAModule->getFullName(), otherOMNeTInterface->omnetARAModule->getFullName()) == 0
            && strcmp(gateToARP->getFullName(), otherOMNeTInterface->gateToARP->getFullName()) == 0;
    }
}

OMNETARA_NAMESPACE_END
