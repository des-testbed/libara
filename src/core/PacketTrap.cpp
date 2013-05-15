/*
 * $FU-Copyright$
 */

#include "PacketTrap.h"

ARA_NAMESPACE_BEGIN

using namespace std;
typedef unordered_set<Packet*, PacketHash, PacketPredicate> PacketSet;
typedef std::deque<Packet*> PacketList;

PacketTrap::PacketTrap(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

PacketTrap::~PacketTrap() {
    // delete all packets that might still be trapped
    TrappedPacketsMap::iterator iterator;
    for (iterator=trappedPackets.begin(); iterator!=trappedPackets.end(); iterator++) {
        pair<AddressPtr, PacketSet*> entryPair = *iterator;
        PacketSet* packetSet = entryPair.second;

        for(auto& packet: *packetSet) {
            delete packet;
        }
        delete packetSet;
    }
    trappedPackets.clear();
}

bool PacketTrap::thereIsAHashSetFor(AddressPtr destination) {
    return trappedPackets.find(destination) != trappedPackets.end();
}

void PacketTrap::trapPacket(Packet* packet) {
    AddressPtr destination = packet->getDestination();
    if(thereIsAHashSetFor(destination) == false) {
        PacketSet* newHashSet = new PacketSet();
        trappedPackets[destination] = newHashSet;
    }

    trappedPackets[destination]->insert(packet);
}

void PacketTrap::untrapPacket(Packet* packet) {
    AddressPtr packetDestination = packet->getDestination();
    TrappedPacketsMap::const_iterator found = trappedPackets.find(packetDestination);
    if(found != trappedPackets.end()) {
        PacketSet* packetSet = found->second;
        PacketSet::const_iterator storedPacketIterator = packetSet->find(packet);
        if(storedPacketIterator != packetSet->end()) {
            Packet* storedPacket = *storedPacketIterator;
            packetSet->erase(storedPacket);

            // if this was the last packet for this destination we need to delete the set
            if(packetSet->size() == 0) {
                trappedPackets.erase(packetDestination);
                delete packetSet;
            }
        }
        else {
            //TODO throw Exception if there is no such trapped packet
        }
    }
    else {
        //TODO throw Exception if there are no packets for this destination
    }
}

bool PacketTrap::contains(Packet* packet) {
    AddressPtr packetDestination = packet->getDestination();
    TrappedPacketsMap::const_iterator found = trappedPackets.find(packetDestination);
    if(found != trappedPackets.end()) {
        PacketSet* packetSet = found->second;
        return packetSet->find(packet) != packetSet->end();
    }
    else {
        return false;
    }
}

bool PacketTrap::isEmpty() {
    return trappedPackets.size() == 0;
}

deque<Packet*>* PacketTrap::getDeliverablePackets(AddressPtr destination) {
    deque<Packet*>* deliverablePackets = new deque<Packet*>();

    TrappedPacketsMap::const_iterator packetsForDestination = trappedPackets.find(destination);
    if(packetsForDestination != trappedPackets.end()) {
        if(routingTable->isDeliverable(destination)) {
            // Add all packets for this destination
            PacketSet* packets = packetsForDestination->second;
            for(auto& trappedPacket: *packets) {
                deliverablePackets->push_back(trappedPacket);
            }
        }
    }

    return deliverablePackets;
}

void PacketTrap::setRoutingTable(RoutingTable *routingTable){
    this->routingTable = routingTable;
}

PacketList PacketTrap::removePacketsForDestination(AddressPtr destination) {
    PacketList removedPackets = PacketList();
    TrappedPacketsMap::const_iterator packetsForDestination = trappedPackets.find(destination);

    if(packetsForDestination != trappedPackets.end()) {
        PacketSet* packetSet = packetsForDestination->second;

        for(auto& packet: *packetSet) {
            removedPackets.push_back(packet);
        }

        trappedPackets.erase(packetsForDestination);
        delete packetSet;
    }

    return removedPackets;
}

unsigned int PacketTrap::getNumberOfTrappedPackets(AddressPtr destination) {
    unsigned int result = 0;

    for (TrappedPacketsMap::iterator entryPair=trappedPackets.begin(); entryPair!=trappedPackets.end(); entryPair++) {
        if(destination == nullptr || destination->equals(entryPair->first)) {
            PacketSet* packetSet = entryPair->second;
            result += packetSet->size();
        }
    }

    return result;
}

ARA_NAMESPACE_END
