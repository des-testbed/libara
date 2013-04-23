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

struct RoutingTableEntryTupel {
    AddressPtr destination;
    RoutingTableEntry* entry;
};

#define RoutingTableEntryList std::deque<RoutingTableEntry*>

class RoutingTable {

public:
    RoutingTable();
    virtual ~RoutingTable();

    /**
     * Get the pheromone value for a specific route in this routing table.
     */
    float getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);
   
    void update(AddressPtr destination, RoutingTableEntry* entry);
    void update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue);

    void removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);
    RoutingTableEntryList getPossibleNextHops(AddressPtr destination);
    RoutingTableEntryList getPossibleNextHops(const Packet* packet);

    /**
     * Returns true if there is a known route to a given destination, else false
     */
    bool isDeliverable(AddressPtr destination);

    /**
     * Checks if a route to the packet destination exists, that does *not* lead
     * over the packets sender.
     */
    bool isDeliverable(const Packet* packet);

    /**
     * The method checks if an destination/nextHop/interface entry already exists.
     */
    bool exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);

    /**
     * A convenience method which returns the negated result of RoutingTable::exists(...)
     * @see RoutingTable::exists
     */
    bool isNewRoute(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);

    void setEvaporationPolicy(EvaporationPolicy* policy);
    EvaporationPolicy* getEvaporationPolicy() const;

    /**
     * Returns the total number of all entries in this table.
     * This is the sum of all routes to all destinations.
     */
    unsigned int getTotalNumberOfEntries() const;

    /**
     * Returns the n'th ~RoutingTableEntry.
     * This method is only used to display the routing table entries to the user.
     */
    RoutingTableEntryTupel getEntryAt(int wantedPosition) const;

protected:
    bool hasTableBeenAccessedEarlier();
    void triggerEvaporation();
    virtual void updateExistingEntry(RoutingTableEntry* oldEntry, RoutingTableEntry* newEntry);
    Time* lastAccessTime;

    std::unordered_map<std::shared_ptr<Address>, RoutingTableEntryList*, AddressHash, AddressPredicate> table;

    /**
     * The memory management of the evaporationPolicy member is handled in class
     * ARA. Thus, there is no delete call to the evaporationPolicy member.
     */
    EvaporationPolicy* evaporationPolicy;
};

ARA_NAMESPACE_END
#endif /* ROUTINGTABLE_H_ */
