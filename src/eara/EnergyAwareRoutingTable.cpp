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
        return 1;
    }
    else {
        return entry->getEnergyValue();
    }
}

ARA_NAMESPACE_END
