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

void ReliableNetworkInterface::receive(Packet* packet) {
    if (packet->getType() != PacketType::ACK) {
        handleNonAckPacket(packet);
    }
    else {
        handleAckPacket(packet);
    }
}

void ReliableNetworkInterface::handleNonAckPacket(Packet* packet) {
    AddressPtr destination = packet->getDestination();

    if(isBroadcastAddress(destination) == false) {
        Packet* ackPacket = packet->createAcknowledgment();
        send(ackPacket, packet->getSender());
    }

    client->receivePacket(packet, this);
}

void ReliableNetworkInterface::handleAckPacket(Packet* ackPacket) {
    unsigned int acknowledgedSeqNr = ackPacket->getSequenceNumber();
    AddressPtr acknowledgedSource = ackPacket->getSource();
    for (std::deque<const Packet*>::iterator iterator = unacknowledgedPackets.begin(); iterator!=unacknowledgedPackets.end(); iterator++) {
        const Packet* currentPacket = *iterator;

        if (currentPacket->getSequenceNumber() == acknowledgedSeqNr && currentPacket->getSource()->equals(acknowledgedSource)) {
            // remove the packet from the list of unacknowledged packets
            unacknowledgedPackets.erase(iterator);

            // free the memory
            delete currentPacket;

            // break out of the for loop
            break;
        }
    }

    delete ackPacket;
}

std::deque<const Packet*> ReliableNetworkInterface::getUnacknowledgedPackets() const {
    return unacknowledgedPackets;
}
} /* namespace ARA */
