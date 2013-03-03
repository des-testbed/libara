/*
 * $FU-Copyright$
 */

#include "PacketTrap.h"

using namespace std;

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;
typedef unordered_set<Packet*, PacketHash, PacketPredicate> PacketSet;

PacketTrap::PacketTrap(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

PacketTrap::~PacketTrap() {
    // delete all packets that might still be trapped
    unordered_map<AddressPtr, PacketSet*>::iterator iterator;
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

    trappedPackets[destination]->insert(packet->clone());
}

void PacketTrap::untrapPacket(Packet* packet) {
    AddressPtr packetDestination = packet->getDestination();
    unordered_map<AddressPtr, PacketSet*>::const_iterator found = trappedPackets.find(packetDestination);
    if(found != trappedPackets.end()) {
        PacketSet* packetSet = found->second;
        PacketSet::const_iterator storedPacketIterator = packetSet->find(packet);
        if(storedPacketIterator != packetSet->end()) {
            Packet* storedPacket = *storedPacketIterator;
            packetSet->erase(storedPacket);
            delete storedPacket;

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
    unordered_map<AddressPtr, PacketSet*>::const_iterator found = trappedPackets.find(packetDestination);
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

deque<Packet*>* PacketTrap::getDeliverablePackets() {
    deque<Packet*>* deliverablePackets = new deque<Packet*>();

    unordered_map<AddressPtr, PacketSet*>::iterator iterator;
    for (iterator=trappedPackets.begin(); iterator!=trappedPackets.end(); iterator++) {
        pair<AddressPtr, PacketSet*> entryPair = *iterator;
        AddressPtr address = entryPair.first;

        if(routingTable->isDeliverable(address)) {
            // Add all packets for this destination
            PacketSet* packets = entryPair.second;
            for(auto& trappedPacket: *packets) {
                deliverablePackets->push_back(trappedPacket);
            }
        }
    }

    return deliverablePackets;
}

// TODO: checks if this is a problem
void PacketTrap::setRoutingTable(RoutingTable *routingTable){
    this->routingTable = routingTable;
}

} /* namespace ARA */
