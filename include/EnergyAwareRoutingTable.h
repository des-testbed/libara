/*
 * $FU-Copyright$
 */

#ifndef ENERGY_AWARE_ROUTINGTABLE_H_
#define ENERGY_AWARE_ROUTINGTABLE_H_

#include "Packet.h"
#include "Address.h"
#include "NetworkInterface.h"
#include "EvaporationPolicy.h"
#include "EnergyAwareRoutingTableEntry.h"
#include "LinearEvaporationPolicy.h"
#include "TimeFactory.h"

#include <deque>
#include <unordered_map>

namespace ARA {

class EnergyAwareRoutingTable {

public:
    EnergyAwareRoutingTable(TimeFactory* timeFactory);
    ~EnergyAwareRoutingTable();

    float getEnergyValue(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
    void update(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface, float pheromoneValue, float energyValue);
    void removeEntry(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
    std::deque<EnergyAwareRoutingTableEntry*>* getPossibleNextHops(std::shared_ptr<Address> destination);
    std::deque<EnergyAwareRoutingTableEntry*>* getPossibleNextHops(const Packet* packet);
    bool isDeliverable(std::shared_ptr<Address> destination);
    bool isDeliverable(const Packet* packet);

    bool exists(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);

    void setEvaporationPolicy(EvaporationPolicy *policy);
    EvaporationPolicy *getEvaporationPolicy() const;
    std::unordered_map<std::shared_ptr<Address>, std::deque<EnergyAwareRoutingTableEntry*>*, AddressHash, AddressPredicate> getEnergyAwareRoutingTable() const;
    void setEnergyAwareRoutingTable(std::unordered_map<std::shared_ptr<Address>, std::deque<EnergyAwareRoutingTableEntry*>*, AddressHash, AddressPredicate> table);

protected:
    bool hasTableBeenAccessedEarlier();
    void triggerEvaporation();

    TimeFactory* timeFactory;
    Time *lastAccessTime;

private:
    std::unordered_map<std::shared_ptr<Address>, std::deque<EnergyAwareRoutingTableEntry*>*, AddressHash, AddressPredicate> table;
    /**
     * The memory management of the evaporationPolicy member is handled in class
     * OMNeTEARAClient. Thus, there is no delete call to the evaporationPolicy member.
     */
    EvaporationPolicy *evaporationPolicy;
};

} /* namespace ARA */
#endif /* ROUTINGTABLE_H_ */
