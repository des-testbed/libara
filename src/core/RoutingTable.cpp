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

// TODO needs short review and confirmation I deleted everything properly
RoutingTable::~RoutingTable() {
    std::unordered_map<Address*, LinkedList<RoutingTableEntry>*, AddressHash, AddressPredicate>::iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        std::pair<Address* const, LinkedList<RoutingTableEntry>*> entryPair = *iterator;
        LinkedList<RoutingTableEntry>* entryList = entryPair.second;
        delete entryList;
    }
    table.clear();
}

void RoutingTable::update(Address* destination, Address* nextHop, NetworkInterface* interface, float pheromoneValue) {
    if(isDeliverable(destination) == false) {
        // this destination is not yet registered
        RoutingTableEntry* newEntry = new RoutingTableEntry(nextHop, interface, pheromoneValue);
        LinkedList<RoutingTableEntry>* entryList = new LinkedList<RoutingTableEntry>();
        table[destination] = entryList;
        entryList->add(newEntry);
    }
    else {
        // there is at least one registered route for this destination
        LinkedList<RoutingTableEntry>* entryList = table[destination];
        bool entryHasBeenUpdated = false;
        // TODO replace this with an iterator
        unsigned int nrOfEntries = entryList->size();
        for (unsigned int i = 0; i < nrOfEntries; ++i) {
            RoutingTableEntry* entry = entryList->get(i);
            if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                entry->setPheromoneValue(pheromoneValue);
                entryHasBeenUpdated = true;
            }
        }

        if(entryHasBeenUpdated == false) {
            RoutingTableEntry* newEntry = new RoutingTableEntry(nextHop, interface, pheromoneValue);
            entryList->add(newEntry);
        }
    }
}

LinkedList<RoutingTableEntry>* RoutingTable::getPossibleNextHops(Address* destination) {
    if(isDeliverable(destination)) {
        // FIXME this should return a copy of this list (to avoid that entries in this list are accidentally removed from or added)
        return table[destination];
    }
    else {
        return new LinkedList<RoutingTableEntry>();
    }
}

LinkedList<RoutingTableEntry>* RoutingTable::getPossibleNextHops(Packet* packet) {
    return getPossibleNextHops(packet->getDestination());
}

bool RoutingTable::isDeliverable(Address* destination) {
    return table.find(destination) != table.end();
}

bool RoutingTable::isDeliverable(Packet* packet) {
    return isDeliverable(packet->getDestination());
}

} /* namespace ARA */
