/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTGate.h"
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
    interfaceID = interfaceEntry->getInterfaceId();
    packetFactory = araClient->getPacketFactory();
    networkConfig = check_and_cast<ARANetworkConfigurator*>(simulation.getModuleByPath("networkConfigurator"));

    // initialize statistics
    nrOfSentControlPackets = 0;
    nrOfSentDataPackets = 0;
    nrOfSentFANTs = 0;
    nrOfSentBANTs = 0;
    nrOfSentPEANTs = 0;
    nrOfSentDuplicateErrors = 0;
    nrOfSentRouteErrors = 0;
    nrOfSentOtherPackets = 0;

    nrOfSentControlBits = 0;
    nrOfSentDataBits = 0;
    nrOfSentFANTBits = 0;
    nrOfSentBANTBits = 0;
    nrOfSentPEANTBits = 0;
    nrOfSentDuplicateErrorBits = 0;
    nrOfSentRouteErrorBits = 0;
    nrOfSentOtherBits = 0;
}

void OMNeTGate::send(const Packet* packet, AddressPtr recipient) {
    send(packet, recipient, omnetARAModule->par("uniCastDelay").doubleValue());
}

void OMNeTGate::send(const Packet* packet, AddressPtr recipient, double sendDelay) {
    OMNeTAddressPtr nextHopAddress = getNextHopAddress(recipient);

    // first remove the control info from the lower level (Ieee802Ctrl)
    Packet* pckt = const_cast<Packet*>(packet);
    cPacket* simPacket = dynamic_cast<cPacket*>(pckt);

    if (simPacket) {
        ASSERT2(simPacket, "Model error: OMNeTGate could not cast the packet to cPacket for sending it via the simulation");
        simPacket->removeControlInfo();

        // then fill in the control info (our routing decision)
        MACAddress macOfNextHop;
        if (isBroadcastAddress(nextHopAddress)) {
            macOfNextHop = MACAddress::BROADCAST_ADDRESS;
        } else {
            macOfNextHop = networkConfig->getMACAddressByIP(*(nextHopAddress.get()));
        }

        Ieee802Ctrl* controlInfo = new Ieee802Ctrl();
        controlInfo->setDest(macOfNextHop);
        simPacket->setControlInfo(controlInfo);

        collectStatistics(packet, simPacket);

        // we might have switched the context from the OMNeTTimer
        omnetARAModule->takeAndSend(simPacket, outGate, sendDelay);
    }
}

void OMNeTGate::collectStatistics(const Packet* packet, cPacket* simPacket) {
    if (packet->isDataPacket()) {
        nrOfSentDataBits += simPacket->getBitLength();
        nrOfSentDataPackets++;
    }
    else {
        nrOfSentControlPackets++;

        int64 packetBitSize = simPacket->getBitLength();
        nrOfSentControlBits += packetBitSize;

        switch (packet->getType()) {
            case PacketType::FANT:
                nrOfSentFANTs++;
                nrOfSentFANTBits += packetBitSize;
                break;
            case PacketType::BANT:
                nrOfSentBANTs++;
                nrOfSentBANTBits += packetBitSize;
                break;
            case PacketType::PEANT:
                nrOfSentPEANTs++;
                nrOfSentPEANTBits += packetBitSize;
                break;
            case PacketType::DUPLICATE_ERROR:
                nrOfSentDuplicateErrors++;
                nrOfSentDuplicateErrorBits += packetBitSize;
                break;
            case PacketType::ROUTE_FAILURE:
                nrOfSentRouteErrors++;
                nrOfSentRouteErrorBits += packetBitSize;
                break;
            default:
                nrOfSentOtherPackets++;
                nrOfSentOtherBits += packetBitSize;
                break;
        }
    }
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

    if(otherOMNeTInterface) {
        return strcmp(omnetARAModule->getFullName(), otherOMNeTInterface->omnetARAModule->getFullName()) == 0
            && strcmp(outGate->getFullName(), otherOMNeTInterface->outGate->getFullName()) == 0;
    } 

    return false;
}

OMNETARA_NAMESPACE_END
