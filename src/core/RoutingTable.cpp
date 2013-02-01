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

#include <iostream>

namespace ARA {

typedef std::shared_ptr<Address> AddressPtr;

RoutingTable::RoutingTable( ){ 
    this->mEvaporationPolicy = new LinearEvaporationPolicy();
}

RoutingTable::RoutingTable(EvaporationPolicy *pEvaporationPolicy):mEvaporationPolicy(pEvaporationPolicy){ }

RoutingTable::~RoutingTable() {
    std::unordered_map<AddressPtr, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate>::iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        std::pair<AddressPtr const, std::deque<RoutingTableEntry*>*> entryPair = *iterator;
        std::deque<RoutingTableEntry*>* entryList = entryPair.second;
        // delete all RoutingTableEntries in the List
        while(entryList->empty() == false) {
            RoutingTableEntry* entry = entryList->back();
            entryList->pop_back();
            delete entry;
        }
        delete entryList;
    }
    table.clear();

    delete mEvaporationPolicy;
}

void RoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue) {
    if(isDeliverable(destination) == false) {
        // this destination is not yet registered
        RoutingTableEntry* newEntry = new RoutingTableEntry(nextHop, interface, pheromoneValue);
        std::deque<RoutingTableEntry*>* entryList = new std::deque<RoutingTableEntry*>();
        entryList->push_back(newEntry);
        table[destination] = entryList;
    }
    else {
        // there is at least one registered route for this destination
        std::deque<RoutingTableEntry*>* entryList = table[destination];
        bool entryHasBeenUpdated = false;
        for (auto& entry: *entryList) {
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

void RoutingTable::removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    if(isDeliverable(destination)) {
        std::deque<RoutingTableEntry*>* entryList = table[destination];
        std::deque<RoutingTableEntry*>::iterator iterator = entryList->begin();
        while(iterator != entryList->end()) {
            RoutingTableEntry* entry = *iterator;
            if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                entryList->erase(iterator);
                delete entry;
                return;
            }
            iterator++;
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

/**
 * TODO: The method should throw an exception if the host can not be found
 */
float RoutingTable::getPheromoneValue(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface) {
    if(isDeliverable(destination)) {
        std::deque<RoutingTableEntry*>* entryList = table[destination];
        for (auto& entry: *entryList) {
            if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                return entry->getPheromoneValue();
            }
        }
    }

    return 0;
}

/**
 * The method checks if an destination/nextHop/interface entry already exists.
 */
bool RoutingTable::exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface){
    if(isDeliverable(destination)){
        std::deque<RoutingTableEntry*>* entries = table[destination];
        for (auto& entry: *entries) {
            if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)){
                delete entries;
                return true;
            }
        }
    }
    return false;
}

/**
 *
 */
void RoutingTable::evaporatePheromones(){
    std::unordered_map<AddressPtr, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate>::iterator i;
    /// iterate over the destination entries
    for(i = table.begin(); i != table.end(); i++){
       std::deque<RoutingTableEntry*>::iterator j;
       /// iterate over the possible next hop entries of a destination
       for(j = (*i).second->begin(); j != (*i).second->end(); j++){
          /// TODO: remove the '1' and replace it by a function parameter
          float phi = this->mEvaporationPolicy->evaporate((*j)->getPheromoneValue(), 1);
          /// update the entry
          if(phi != 0.0){
              (*j)->setPheromoneValue(phi);
          // remove the entry from the list (CHECK IF THAT'S A GOOD IDEA)
          }else{
              /// todo
          }
       }
    }
}

} /* namespace ARA */
