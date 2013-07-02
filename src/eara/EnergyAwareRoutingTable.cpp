/*
 * $FU-Copyright$
 */

#include "EnergyAwareRoutingTable.h"
#include "EARARoutingTableEntry.h"

ARA_NAMESPACE_BEGIN

void EnergyAwareRoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue, float normalizedEnergyValue) {
    EARARoutingTableEntry* entry = new EARARoutingTableEntry(nextHop, interface, pheromoneValue, normalizedEnergyValue);
    RoutingTable::update(destination, entry);
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
