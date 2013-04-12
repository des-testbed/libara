/*
 * $FU-Copyright$
 */

#ifndef ROUTINGTABLE_H_
#define ROUTINGTABLE_H_

#include "ARAMacros.h"
#include "Packet.h"
#include "Address.h"
#include "NetworkInterface.h"
#include "EvaporationPolicy.h"
#include "RoutingTableEntry.h"
#include "ExponentialEvaporationPolicy.h"
#include "Clock.h"

#include <deque>
#include <unordered_map>

ARA_NAMESPACE_BEGIN

class RoutingTable {

public:
    RoutingTable();
    virtual ~RoutingTable();

    float getPheromoneValue(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);

   
    void update(std::shared_ptr<Address> destination, RoutingTableEntry* entry);
    void update(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface, float pheromoneValue);

    void removeEntry(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
    std::deque<RoutingTableEntry*> getPossibleNextHops(std::shared_ptr<Address> destination);
    std::deque<RoutingTableEntry*> getPossibleNextHops(const Packet* packet);
    bool isDeliverable(std::shared_ptr<Address> destination);

    /**
     * Checks if a route to the packet destination exists, that does *not* lead
     * over the packets sender.
     */
    bool isDeliverable(const Packet* packet);

    /**
     * The method checks if an destination/nextHop/interface entry already exists.
     */
    bool exists(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
    bool isNewRoute(std::shared_ptr<Address> destination, std::shared_ptr<Address> nextHop, NetworkInterface* interface);

    void setEvaporationPolicy(EvaporationPolicy *policy);
    EvaporationPolicy *getEvaporationPolicy() const;

protected:
    bool hasTableBeenAccessedEarlier();
    void triggerEvaporation();
    virtual void updateExistingEntry(RoutingTableEntry *oldEntry, RoutingTableEntry *newEntry);
    Time *lastAccessTime;

    std::unordered_map<std::shared_ptr<Address>, std::deque<RoutingTableEntry*>*, AddressHash, AddressPredicate> table;
    /**
     * The memory management of the evaporationPolicy member is handled in class
     * ARA. Thus, there is no delete call to the evaporationPolicy member.
     */
    EvaporationPolicy *evaporationPolicy;
};

ARA_NAMESPACE_END
#endif /* ROUTINGTABLE_H_ */
