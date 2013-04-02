/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_ROUTINGTABLE_H_
#define ENERGY_AWARE_ROUTINGTABLE_H_

#include "RoutingTable.h"
#include "Address.h"

#include <unordered_map>

namespace ARA {
    typedef std::shared_ptr<Address> AddressPtr;

    class EnergyAwareRoutingTable : public RoutingTable {
        public:
            void update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue, unsigned char energyValue);
            void updateEnergyOfNode(AddressPtr nodeAddress, unsigned char newEnergyLevel);
            bool hasEnergyInformationFor(AddressPtr nodeAddress);
            unsigned char getEnergyValueOf(AddressPtr nodeAddress);

        protected:
            std::unordered_map<AddressPtr, unsigned char, AddressHash, AddressPredicate> neighborEnergyValues;
};

} /* namespace ARA */
#endif /* ROUTINGTABLE_H_ */
