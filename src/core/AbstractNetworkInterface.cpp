/*
 * $FU-Copyright$
 */

#include "AbstractNetworkInterface.h"

namespace ARA {

AbstractNetworkInterface::AbstractNetworkInterface(AbstractARAClient* client, AddressPtr localAddress, AddressPtr broadcastAddress) {
    this->client = client;
    this->localAddress = localAddress;
    this->broadcastAddress = broadcastAddress;
}

AddressPtr AbstractNetworkInterface::getLocalAddress() const {
    return localAddress;
}

bool AbstractNetworkInterface::isBroadcastAddress(AddressPtr someAddress) const {
    return someAddress->equals(broadcastAddress);
}

bool AbstractNetworkInterface::isLocalAddress(AddressPtr someAddress) const {
    return someAddress->equals(localAddress);
}

void AbstractNetworkInterface::broadcast(const Packet* packet) {
    send(packet, broadcastAddress);
}

void AbstractNetworkInterface::receive(Packet* packet) {
    // standard implementation which might be overwritten in the implementing classes.
    deliverToARAClient(packet);
}

void AbstractNetworkInterface::deliverToARAClient(Packet* packet) {
    client->receivePacket(packet, this);
}

} /* namespace ARA */
