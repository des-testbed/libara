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
typedef std::unordered_map<AddressPtr, std::shared_ptr<RoutingTableEntryList>, AddressHash, AddressPredicate> RoutingTableMap;

class RoutingTable {

public:
    RoutingTable();
    virtual ~RoutingTable();

    /**
     * Get the pheromone value for a specific route in this routing table.
     */
    float getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);
   
    virtual void update(AddressPtr destination, RoutingTableEntry* entry);
    virtual void update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue);

    void removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);
    RoutingTableEntryList getPossibleNextHops(const Packet* packet);
    RoutingTableEntryList getPossibleNextHops(AddressPtr destination);

    /**
     * Checks if a route to the packet destination exists, that does *not* lead
     * over the packets sender.
     */
    bool isDeliverable(const Packet* packet);

    /**
     * Returns true if there is at least one stored route for the given destination in this routing table.
     */
    bool isDeliverable(AddressPtr destination);

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
    virtual unsigned int getTotalNumberOfEntries() const;

    /**
     * Returns the n'th ~RoutingTableEntry.
     * This method is only used to display the routing table entries to the user.
     */
    RoutingTableEntryTupel getEntryAt(int position) const;

    /**
     * Triggers the evaporation process if enough time since the last evaporation has passed.
     */
    virtual void triggerEvaporation();

    /**
     * Returns all known routes that lead over the given next hop.
     */
    std::deque<RoutingTableEntryTupel> getAllRoutesThatLeadOver(AddressPtr nextHop) const;

    /**
     *
     */
    virtual std::string toString(int position=-1);

    /**
     * The method provides an output stream for the routing table by overloading
     * the '<<' operator.
     *
     */
    friend std::ostream& operator<<(std::ostream& out, RoutingTable& routingTable) {
        RoutingTableMap::const_iterator i;

        for (i = routingTable.table.begin(); i != routingTable.table.end(); i++) {
            AddressPtr destination = i->first;
            std::shared_ptr<RoutingTableEntryList> entryList = i->second;

            for (auto& entry: *entryList) {
              out << "[destination] " << destination << " " << entry;
            }
         }

         return out;
    }

private:
    void applyEvaporation(Time* currentTime);

protected:
    bool hasTableBeenAccessedEarlier();
    virtual void updateExistingEntry(RoutingTableEntry* oldEntry, RoutingTableEntry* newEntry);
    Time* lastAccessTime;

    RoutingTableMap table;

    /**
     * The memory management of the evaporationPolicy member is handled in class
     * ARA. Thus, there is no delete call to the evaporationPolicy member.
     */
    EvaporationPolicy* evaporationPolicy;

};

ARA_NAMESPACE_END
#endif /* ROUTINGTABLE_H_ */
