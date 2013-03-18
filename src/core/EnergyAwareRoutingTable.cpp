/*
 * $FU-Copyright$
 */

#include "EnergyAwareRoutingTable.h"

namespace ARA {
    typedef std::shared_ptr<Address> AddressPtr;

    void EnergyAwareRoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue, float energyValue){
        EnergyAwareRoutingTableEntry *entry = new EnergyAwareRoutingTableEntry(nextHop, interface, pheromoneValue, energyValue);
        RoutingTable::update(destination, entry);
    }

    void EnergyAwareRoutingTable::updateExistingEntry(RoutingTableEntry *oldEntry, RoutingTableEntry *newEntry){
        try {
            EnergyAwareRoutingTableEntry *o  = static_cast<EnergyAwareRoutingTableEntry*>(oldEntry);
            EnergyAwareRoutingTableEntry *n  = static_cast<EnergyAwareRoutingTableEntry*>(newEntry);

            o->setPheromoneValue(n->getPheromoneValue());
            o->setEnergyValue(n->getEnergyValue());

        } catch (std::bad_cast &exp) {
            /// FIXME: add better exception handling
        }
    }

    float EnergyAwareRoutingTable::getEnergyValue(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface) {
        triggerEvaporation();

        if(isDeliverable(destination)) {
<<<<<<< HEAD
            std::deque<RoutingTableEntry*>* entryList = getPossibleNextHops(destination);
=======
            std::deque<RoutingTableEntry*>* entryList = (getRoutingTable())[destination];
>>>>>>> 02b19ac7086df67a043277a98cb5532f9f7c08f9

            for (auto& entry: *entryList) {
                if(entry->getAddress()->equals(nextHop) && entry->getNetworkInterface()->equals(interface)) {
                    try {
                        EnergyAwareRoutingTableEntry *e = static_cast<EnergyAwareRoutingTableEntry*>(entry);
				        return e->getEnergyValue();
                    } catch (std::bad_cast &exp) {
                        /// FIXME: add better exception handling
                        return -1;
                    }
                }
            }
        }

        return 0;
    }
}
