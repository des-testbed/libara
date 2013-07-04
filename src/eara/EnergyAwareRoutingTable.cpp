/*
 * $FU-Copyright$
 */

#include "EnergyAwareRoutingTable.h"
#include "EARARoutingTableEntry.h"

ARA_NAMESPACE_BEGIN

void EnergyAwareRoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue) {
    if(isDeliverable(destination)) {
        RoutingTableEntryList* entries = table[destination];
        for (auto& entry: *entries) {
            if (entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                entry->setPheromoneValue(pheromoneValue);
                return;
            }
        }
    }

    // if no entry does yet exist, create a new one with energy default to 1
    EARARoutingTableEntry* entry = new EARARoutingTableEntry(nextHop, interface, pheromoneValue, 1);
    RoutingTable::update(destination, entry);
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

float EnergyAwareRoutingTable::getEnergyValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface) {
    if(isDeliverable(destination)) {
        RoutingTableEntryList* entryList = table[destination];
        for (auto& entry: *entryList) {
            if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                EARARoutingTableEntry* earaEntry = (EARARoutingTableEntry*) entry;
                return earaEntry->getEnergyValue();
            }
        }
    }

    return 1;
}

ARA_NAMESPACE_END
