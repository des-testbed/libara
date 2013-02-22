/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

#include "PacketTrap.h"

using namespace std;

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;
typedef unordered_set<const Packet*, PacketHash, PacketPredicate> PacketSet;

PacketTrap::PacketTrap(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}
PacketTrap::~PacketTrap() {
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

void PacketTrap::trapPacket(const Packet* packet) {
    AddressPtr destination = packet->getDestination();
    if(thereIsAHashSetFor(destination) == false) {
        PacketSet* newHashSet = new PacketSet();
        trappedPackets[destination] = newHashSet;
    }

    trappedPackets[destination]->insert(packet->clone());
}

void PacketTrap::untrapPacket(const Packet* packet) {
    AddressPtr packetDestination = packet->getDestination();
    unordered_map<AddressPtr, PacketSet*>::const_iterator found = trappedPackets.find(packetDestination);
    if(found != trappedPackets.end()) {
        PacketSet* packetSet = found->second;
        PacketSet::const_iterator storedPacketIterator = packetSet->find(packet);
        if(storedPacketIterator != packetSet->end()) {
            const Packet* storedPacket = *storedPacketIterator;
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

deque<const Packet*>* PacketTrap::getDeliverablePackets() {
    deque<const Packet*>* deliverablePackets = new deque<const Packet*>();

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
