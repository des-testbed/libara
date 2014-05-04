/*
 * $FU-Copyright$
 */
#ifndef TESTBED_ROUTINGTABLE_H_
#define TESTBED_ROUTINGTABLE_H_

#include "ARAMacros.h"
#include "Packet.h"
#include "Address.h"
#include "Testbed.h" 
#include "NetworkInterface.h"
#include "EvaporationPolicy.h"
#include "RoutingTable.h"
#include "ExponentialEvaporationPolicy.h"
#include "RoutingTable.h"
#include "Clock.h"

#include <mutex>

TESTBED_NAMESPACE_BEGIN

/**
 * The TestbedRoutingTable is a concurrent version of class RoutingTable. This
 * means that no functionality is altered, but mutex and lock guards are 
 * introduced.
 */
class TestbedRoutingTable : public RoutingTable {
    public:
        TestbedRoutingTable();
        virtual ~TestbedRoutingTable();

        float getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);
        void update(AddressPtr destination, RoutingTableEntry* entry);
        void removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);
        
        bool isDeliverable(AddressPtr destination);
        bool isDeliverable(const Packet* packet);

        bool exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);
        RoutingTableEntryList getPossibleNextHops(const Packet* packet);
        RoutingTableEntryList getPossibleNextHops(AddressPtr destination);

        void triggerEvaporation();
	unsigned int getTotalNumberOfEntries() const;
        RoutingTableEntryTupel getEntryAt(int position) const;
        std::deque<RoutingTableEntryTupel> getAllRoutesThatLeadOver(AddressPtr nextHop) const;

    private:
	mutable std::mutex mutex;
};

TESTBED_NAMESPACE_END
#endif /* TESTBED_ROUTINGTABLE_H_ */
