/*
 * $FU-Copyright$
 */

#include "EnergyAwareRoutingTable.h"
#include "Exception.h"

ARA_NAMESPACE_BEGIN

void EnergyAwareRoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue) {
    EARARoutingTableEntry* entry = getRoutingTableEntry(destination, nextHop, interface);
    if (entry == nullptr) {
        // if no entry does yet exist, create a new one with energy default to 1
        entry = new EARARoutingTableEntry(nextHop, interface, pheromoneValue, 1);
        RoutingTable::update(destination, entry);
    }
    else {
        entry->setPheromoneValue(pheromoneValue);
    }
}

void EnergyAwareRoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue, float normalizedEnergyValue) {
    EARARoutingTableEntry* entry = new EARARoutingTableEntry(nextHop, interface, pheromoneValue, normalizedEnergyValue);
    RoutingTable::update(destination, entry);
}

void EnergyAwareRoutingTable::updateExistingEntry(RoutingTableEntry* oldEntry, RoutingTableEntry* newEntry) {
    //TODO somehow do this without casting (needs bigger refactoring of the RoutingTable class I guess)
    oldEntry->setPheromoneValue(newEntry->getPheromoneValue());

    EARARoutingTableEntry* oldEARAEntry = (EARARoutingTableEntry*) oldEntry;
    EARARoutingTableEntry* newEARAEntry = (EARARoutingTableEntry*) newEntry;
    oldEARAEntry->setEnergyValue(newEARAEntry->getEnergyValue());
}

bool EnergyAwareRoutingTable::updateEnergyValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float newEnergyValue) {
    EARARoutingTableEntry* entry = getRoutingTableEntry(destination, nextHop, interface);
    if (entry != nullptr) {
        entry->setEnergyValue(newEnergyValue);
        return true;
    }
    else {
        return false;
    }
}

EARARoutingTableEntry* EnergyAwareRoutingTable::getRoutingTableEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    if (isDeliverable(destination)) {
        RoutingTableEntryList* entries = table[destination];
        for (auto& entry: *entries) {
            if (entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                //TODO somehow remove this cast (refactor)
                return (EARARoutingTableEntry*) entry;
            }
        }
    }

    // if there is no such entry we return the nullptr
    return nullptr;
}

float EnergyAwareRoutingTable::getEnergyValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    EARARoutingTableEntry* entry = getRoutingTableEntry(destination, nextHop, interface);
    if (entry == nullptr) {
        return 10;
    }
    else {
        return entry->getEnergyValue();
    }
}

EARARoutingTableEntryTupel EnergyAwareRoutingTable::getEntryAt(int wantedPosition) const {
    int currentPosition = 0;
    RoutingTableMap::const_iterator iterator;
    for (iterator=table.begin(); iterator!=table.end(); iterator++) {
        AddressPtr destination = iterator->first;
        RoutingTableEntryList* entryList = iterator->second;
        for (auto& entry: *entryList) {
            if(currentPosition == wantedPosition) {
                EARARoutingTableEntryTupel tupel;
                tupel.destination = destination;
                tupel.entry = (EARARoutingTableEntry*) entry;
                return tupel;
            }
            else {
                currentPosition++;
            }
        }
    }

    throw Exception("EnergyAwareRoutingTable::getEntryAt: Index out of bounds");
}

ARA_NAMESPACE_END
