/*
 * $FU-Copyright$
 */

#include "PacketTrap.h"

ARA_NAMESPACE_BEGIN

using namespace std;
typedef std::deque<Packet*> PacketQueue;

PacketTrap::PacketTrap(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

PacketTrap::~PacketTrap() {
    // delete all packets that might still be trapped
    TrappedPacketsMap::iterator iterator;
    for (iterator=trappedPackets.begin(); iterator!=trappedPackets.end(); iterator++) {
        pair<AddressPtr, PacketQueue> entryPair = *iterator;
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
        PacketQueue newList = PacketList();
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

PacketQueue* PacketTrap::untrapDeliverablePackets(AddressPtr destination) {
    PacketQueue* deliverablePackets = new PacketList();

    TrappedPacketsMap::const_iterator packetsForDestination = trappedPackets.find(destination);
    if(packetsForDestination != trappedPackets.end()) {
        if(routingTable->isDeliverable(destination)) {
            // Add all packets for this destination
            PacketQueue packets = packetsForDestination->second;
            for(auto& trappedPacket: packets) {
                deliverablePackets->push_back(trappedPacket);
            }
            trappedPackets.erase(destination);
        }
    }

    return deliverablePackets;
}

void PacketTrap::setRoutingTable(RoutingTable *routingTable){
    this->routingTable = routingTable;
}

PacketQueue PacketTrap::removePacketsForDestination(AddressPtr destination) {
    PacketQueue removedPackets = PacketList();
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
