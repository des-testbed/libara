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

#include "RoutingTable.h"
#include <utility>

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

RoutingTable::~RoutingTable() {
    std::unordered_map<AddressPtr, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate>::iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        std::pair<AddressPtr const, std::deque<RoutingTableEntry*>*> entryPair = *iterator;
        std::deque<RoutingTableEntry*>* entryList = entryPair.second;

        // delete all RoutingTableEntries in the List
        entryList->erase (entryList->begin(),entryList->end());
        delete entryList;
    }
    table.clear();
}

void RoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue) {
    if(isDeliverable(destination) == false) {
        // this destination is not yet registered
        RoutingTableEntry* newEntry = new RoutingTableEntry(nextHop, interface, pheromoneValue);
        std::deque<RoutingTableEntry*>* entryList = new std::deque<RoutingTableEntry*>();
        table[destination] = entryList;
        entryList->push_back(newEntry);
    }
    else {
        // there is at least one registered route for this destination
        std::deque<RoutingTableEntry*>* entryList = table[destination];
        bool entryHasBeenUpdated = false;
        // TODO replace this with an iterator
        unsigned int nrOfEntries = entryList->size();
        for (unsigned int i = 0; i < nrOfEntries; ++i) {
            RoutingTableEntry* entry = entryList->at(i);
            if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                entry->setPheromoneValue(pheromoneValue);
                entryHasBeenUpdated = true;
            }
        }

        if(entryHasBeenUpdated == false) {
            RoutingTableEntry* newEntry = new RoutingTableEntry(nextHop, interface, pheromoneValue);
            entryList->push_back(newEntry);
        }
    }
}

std::deque<RoutingTableEntry*>* RoutingTable::getPossibleNextHops(AddressPtr destination) {
    if(isDeliverable(destination)) {
        // FIXME this should return a copy of this list (to avoid that entries in this list are accidentally removed from or added)
        return table[destination];
    }
    else {
        return new std::deque<RoutingTableEntry*>();
    }
}

std::deque<RoutingTableEntry*>* RoutingTable::getPossibleNextHops(const Packet* packet) {
    return getPossibleNextHops(packet->getDestination());
}

bool RoutingTable::isDeliverable(AddressPtr destination) {
    return table.find(destination) != table.end();
}

bool RoutingTable::isDeliverable(const Packet* packet) {
    return isDeliverable(packet->getDestination());
}

} /* namespace ARA */
