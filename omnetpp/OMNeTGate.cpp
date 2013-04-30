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
#include "Ieee802Ctrl_m.h"

using namespace std;

OMNETARA_NAMESPACE_BEGIN

OMNeTGate::OMNeTGate(AbstractOMNeTARAClient* module, AbstractARAClient* araClient, cGate* outGate, InterfaceEntry* interfaceEntry) : AbstractNetworkInterface(araClient) {
    this->omnetARAModule = module;
    this->outGate = outGate;

    IPv4Address localAddress = IPvXAddressResolver().getAddressFrom(interfaceEntry, IPvXAddressResolver::ADDR_IPv4).get4();
    IPv4Address netmask = interfaceEntry->ipv4Data()->getNetmask();
    IPv4Address networkAddress = localAddress.doAnd(netmask);
    IPv4Address broadcastAddress = networkAddress.getBroadcastAddress(netmask);

    this->localAddress = shared_ptr<Address>(new OMNeTAddress(localAddress));
    this->broadcastAddress = shared_ptr<Address>(new OMNeTAddress(broadcastAddress));
    this->interfaceID = interfaceEntry->getInterfaceId();
    this->packetFactory = araClient->getPacketFactory();
    this->networkConfig = check_and_cast<ARANetworkConfigurator*>(simulation.getModuleByPath("networkConfigurator"));
}

void OMNeTGate::send(const Packet* packet, shared_ptr<Address> recipient) {
    send(packet, recipient, omnetARAModule->par("uniCastDelay").doubleValue());
}

void OMNeTGate::send(const Packet* packet, shared_ptr<Address> recipient, double sendDelay) {
    OMNeTPacket* omnetPacket = (OMNeTPacket*) packet;
    OMNeTAddressPtr nextHopAddress = getNextHopAddress(recipient);

    // first remove the control info from the lower level (Ieee802Ctrl)
    omnetPacket->removeControlInfo();

    // then fill in the control info (our routing decision)
    MACAddress macOfNextHop = networkConfig->getMACAddressByIP(*(nextHopAddress.get()));
    Ieee802Ctrl* controlInfo = new Ieee802Ctrl();
    controlInfo->setDest(macOfNextHop);
    omnetPacket->setControlInfo(controlInfo);

    // we might have switched the context from the OMNeTTimer
    omnetARAModule->takeAndSend(omnetPacket, outGate, sendDelay);
}

OMNeTAddressPtr OMNeTGate::getNextHopAddress(shared_ptr<Address> recipient) {
    shared_ptr<OMNeTAddress> nextHopAddress (dynamic_pointer_cast<OMNeTAddress>(recipient));
    if(nextHopAddress == nullptr) {
        throw cRuntimeError("Error in OMNeTGate: Can only send packets to OMNeTAddress recipients");
    }
    return nextHopAddress;
}

void OMNeTGate::broadcast(const Packet* packet) {
    send(packet, broadcastAddress, omnetARAModule->par("broadCastDelay").doubleValue());
}

bool OMNeTGate::equals(NetworkInterface* otherInterface) {
    OMNeTGate* otherOMNeTInterface = dynamic_cast<OMNeTGate*>(otherInterface);
    if(otherOMNeTInterface == NULL) {
        return false;
    }
    else {
        return strcmp(omnetARAModule->getFullName(), otherOMNeTInterface->omnetARAModule->getFullName()) == 0
            && strcmp(outGate->getFullName(), otherOMNeTInterface->outGate->getFullName()) == 0;
    }
}

OMNETARA_NAMESPACE_END
