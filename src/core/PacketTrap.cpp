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

PacketTrap::PacketTrap(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}
PacketTrap::~PacketTrap() {
    unordered_map<AddressPtr, unordered_set<const Packet*, PacketHash, PacketPredicate>*>::iterator iterator;
    for (iterator=trappedPackets.begin(); iterator!=trappedPackets.end(); iterator++) {
        pair<AddressPtr, unordered_set<const Packet*, PacketHash, PacketPredicate>*> entryPair = *iterator;
        delete entryPair.second;
    }
    trappedPackets.clear();
}

bool PacketTrap::thereIsAHashSetFor(AddressPtr destination) {
    return trappedPackets.find(destination) != trappedPackets.end();
}

void PacketTrap::trapPacket(const Packet* packet) {
    AddressPtr destination = packet->getDestination();
    if(thereIsAHashSetFor(destination) == false) {
        unordered_set<const Packet*, PacketHash, PacketPredicate>* newHashSet = new unordered_set<const Packet*, PacketHash, PacketPredicate>();
        trappedPackets[destination] = newHashSet;
    }

    trappedPackets[destination]->insert(packet);
}

void PacketTrap::untrapPacket(const Packet* packet) {
    AddressPtr packetDestination = packet->getDestination();
    unordered_map<AddressPtr, unordered_set<const Packet*, PacketHash, PacketPredicate>*>::const_iterator found = trappedPackets.find(packetDestination);
    if(found != trappedPackets.end()) {
        unordered_set<const Packet*, PacketHash, PacketPredicate>* packetSet = found->second;
        packetSet->erase(packet);
        if(packetSet->size() == 0) {
            trappedPackets.erase(packetDestination);
            delete packetSet;
        }
    }
    else {
        //TODO throw Exception if there are no packets for this destination
    }
}

bool PacketTrap::contains(Packet* packet) {
    AddressPtr packetDestination = packet->getDestination();
    unordered_map<AddressPtr, unordered_set<const Packet*, PacketHash, PacketPredicate>*>::const_iterator found = trappedPackets.find(packetDestination);
    if(found != trappedPackets.end()) {
        unordered_set<const Packet*, PacketHash, PacketPredicate>* packetSet = found->second;
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

    unordered_map<AddressPtr, unordered_set<const Packet*, PacketHash, PacketPredicate>*>::iterator iterator;
    for (iterator=trappedPackets.begin(); iterator!=trappedPackets.end(); iterator++) {
        pair<AddressPtr, unordered_set<const Packet*, PacketHash, PacketPredicate>*> entryPair = *iterator;
        AddressPtr address = entryPair.first;

        if(routingTable->isDeliverable(address)) {
            // Add all packets for this destination
            unordered_set<const Packet*, PacketHash, PacketPredicate>* packets = entryPair.second;
            unordered_set<const Packet*>::iterator packetIterator;
            for (packetIterator=packets->begin(); packetIterator!=packets->end(); packetIterator++) {
                const Packet* trappedPacket = *packetIterator;
                deliverablePackets->push_back(trappedPacket);
            }
        }
    }

    return deliverablePackets;
}

} /* namespace ARA */
