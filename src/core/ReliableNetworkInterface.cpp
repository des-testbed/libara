/*
 * $FU-Copyright$
 */

#include "ReliableNetworkInterface.h"

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

ReliableNetworkInterface::ReliableNetworkInterface(AbstractARAClient* client, AddressPtr localAddress, AddressPtr broadcastAddress) : AbstractNetworkInterface(client, localAddress, broadcastAddress) {
    unacknowledgedPackets = std::deque<const Packet*>();
}

ReliableNetworkInterface::~ReliableNetworkInterface() {
    for(auto& packet : unacknowledgedPackets) {
        delete packet;
    }
}

void ReliableNetworkInterface::send(const Packet* packet, std::shared_ptr<Address> recipient) {
    unacknowledgedPackets.push_back(packet);
    doSend(packet, recipient);
}

void ReliableNetworkInterface::broadcast(const Packet* packet) {
    doSend(packet, broadcastAddress);
    delete packet;
}

std::deque<const Packet*> ReliableNetworkInterface::getUnacknowledgedPackets() const {
    return unacknowledgedPackets;
}
} /* namespace ARA */
