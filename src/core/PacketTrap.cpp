/*
 * $FU-Copyright$
 */

#include "PacketTrap.h"

ARA_NAMESPACE_BEGIN

PacketTrap::PacketTrap(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

PacketTrap::~PacketTrap() {
    // delete all packets that might still be trapped
    TrappedPacketsMap::iterator iterator;
    for (iterator=trappedPackets.begin(); iterator!=trappedPackets.end(); iterator++) {
        std::pair<AddressPtr, PacketQueue> entryPair = *iterator;
        PacketQueue packetQueue = entryPair.second;

        for(auto& packet: packetQueue) {
            delete packet;
        }
    }
    trappedPackets.clear();
}

void PacketTrap::trapPacket(Packet* packet) {
    AddressPtr destination = packet->getDestination();
    if(trappedPackets.find(destination) == trappedPackets.end()) {
        PacketQueue newList = PacketQueue();
        trappedPackets[destination] = newList;
    }

    trappedPackets[destination].push_back(packet);
}

bool PacketTrap::contains(Packet* packet) {
    AddressPtr packetDestination = packet->getDestination();
    TrappedPacketsMap::const_iterator found = trappedPackets.find(packetDestination);
    if(found != trappedPackets.end()) {
        PacketQueue packetQueue = found->second;
        for(auto& trappedPacket: packetQueue) {
            if(trappedPacket->equals(packet)) {
                return true;
            }
        }
    }

    return false;
}

bool PacketTrap::isEmpty() {
    return trappedPackets.size() == 0;
}

PacketQueue PacketTrap::untrapDeliverablePackets(AddressPtr destination) {
    TrappedPacketsMap::const_iterator packetsForDestination = trappedPackets.find(destination);
    if(packetsForDestination != trappedPackets.end()) {
        if(routingTable->isDeliverable(destination)) {
            PacketQueue deliverablePackets = packetsForDestination->second;
            trappedPackets.erase(destination);
            return deliverablePackets;
        }
    }

    // if there are no deliverable packets we just return an empty queue
    return PacketQueue();
}

PacketQueue PacketTrap::removePacketsForDestination(AddressPtr destination) {
    PacketQueue removedPackets = PacketQueue();
    TrappedPacketsMap::const_iterator packetsForDestination = trappedPackets.find(destination);

    if(packetsForDestination != trappedPackets.end()) {
        removedPackets = packetsForDestination->second;
        trappedPackets.erase(packetsForDestination);
    }

    return removedPackets;
}

unsigned int PacketTrap::getNumberOfTrappedPackets(AddressPtr destination) {
    unsigned int result = 0;

    for (TrappedPacketsMap::iterator entryPair=trappedPackets.begin(); entryPair!=trappedPackets.end(); entryPair++) {
        if(destination == nullptr || destination->equals(entryPair->first)) {
            PacketQueue packetQueue = entryPair->second;
            result += packetQueue.size();
        }
    }

    return result;
}

ARA_NAMESPACE_END
