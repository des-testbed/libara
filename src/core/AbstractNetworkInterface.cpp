/*
 * $FU-Copyright$
 */

#include "AbstractNetworkInterface.h"

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

AbstractNetworkInterface::AbstractNetworkInterface(AbstractARAClient* client, AddressPtr localAddress, AddressPtr broadcastAddress) {
    this->client = client;
    this->localAddress = localAddress;
    this->broadcastAddress = broadcastAddress;
}

AddressPtr AbstractNetworkInterface::getLocalAddress() const {
    return localAddress;
}

bool AbstractNetworkInterface::isBroadcastAddress(std::shared_ptr<Address> someAddress) const {
    return someAddress->equals(broadcastAddress);
}

void AbstractNetworkInterface::receive(Packet* packet) {
    // standard implementation which might be overwritten in the implementing classes.
    deliverToARAClient(packet);
}

void AbstractNetworkInterface::deliverToARAClient(Packet* packet) {
    client->receivePacket(packet, this);
}

} /* namespace ARA */
