/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTGate.h"
#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/OMNeTAddress.h"
#include "IInterfaceTable.h"
#include "IPv4InterfaceData.h"
#include "IPAddressResolver.h"
#include "IPControlInfo.h"
#include "IPDatagram.h"

using namespace std;

namespace ARA {
namespace omnetpp {

typedef std::shared_ptr<OMNeTAddress> OMNeTAddressPtr;

OMNeTGate::OMNeTGate(OMNeTARA* araClient, cGate* gateToARP, InterfaceEntry* interfaceEntry, double broadCastDelay, double uniCastDelay, int ackTimeoutInMicroSeconds) : ReliableNetworkInterface(araClient, ackTimeoutInMicroSeconds) {
    this->omnetARAClient = araClient;
    this->gateToARP = gateToARP;
    this->broadCastDelay = broadCastDelay;
    this->uniCastDelay = uniCastDelay;

    IPAddress localAddress = IPAddressResolver().getAddressFrom(interfaceEntry, IPAddressResolver::ADDR_IPv4).get4();
    IPAddress netmask = interfaceEntry->ipv4Data()->getNetmask();
    IPAddress networkAddress = localAddress.doAnd(netmask);
    IPAddress broadcastAddress = networkAddress.getBroadcastAddress(netmask);

    this->localAddress = shared_ptr<Address>(new OMNeTAddress(localAddress));
    this->broadcastAddress = shared_ptr<Address>(new OMNeTAddress(broadcastAddress));
    this->interfaceID = interfaceEntry->getInterfaceId();
}

void OMNeTGate::doSend(const Packet* packet, shared_ptr<Address> recipient) {
    doSend(packet, recipient, uniCastDelay);
}

void OMNeTGate::doSend(const Packet* packet, shared_ptr<Address> recipient, double sendDelay) {
    // TODO somehow remove this ugly casting stuff
    OMNeTPacket* originalPacket = (OMNeTPacket*) packet;
    OMNeTPacket* omnetPacket = (OMNeTPacket*) originalPacket->clone();
    OMNeTAddressPtr nextHopAddress = getNextHopAddress(recipient);

    // Get the encapsulated packet (if any)
    if(originalPacket->getEncapsulatedPacket()) {
        cPacket* encapsulatedPacket = originalPacket->decapsulate();
        omnetPacket->encapsulate(encapsulatedPacket);
    }

    IPRoutingDecision* controlInfo = new IPRoutingDecision();
    controlInfo->setNextHopAddr(*(nextHopAddress.get()));
    controlInfo->setInterfaceId(interfaceID);
    omnetPacket->setControlInfo(controlInfo);

    // we might have switched the context from the OMNeTTimer
    omnetARAClient->takeAndSend(omnetPacket, gateToARP, sendDelay);
}

OMNeTAddressPtr OMNeTGate::getNextHopAddress(shared_ptr<Address> recipient) {
    shared_ptr<OMNeTAddress> nextHopAddress (dynamic_pointer_cast<OMNeTAddress>(recipient));
    if(nextHopAddress == nullptr) {
        throw cRuntimeError("Error in OMNeTGate: Can only send packets to OMNeTAddress recipients");
    }
    return nextHopAddress;
}

void OMNeTGate::broadcast(const Packet* packet) {
    doSend(packet, broadcastAddress, broadCastDelay);
}

bool OMNeTGate::equals(NetworkInterface* otherInterface) {
    OMNeTGate* otherOMNeTInterface = dynamic_cast<OMNeTGate*>(otherInterface);
    if(otherOMNeTInterface == NULL) {
        return false;
    }
    else {
        return strcmp(omnetARAClient->getFullName(), otherOMNeTInterface->omnetARAClient->getFullName()) == 0
            && strcmp(gateToARP->getFullName(), otherOMNeTInterface->gateToARP->getFullName()) == 0;
    }
}

} /* namespace omnetpp */
} /* namespace ARA */
