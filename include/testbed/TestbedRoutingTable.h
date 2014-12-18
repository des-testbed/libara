/*
 * $FU-Copyright$
 */
#ifndef TESTBED_ROUTINGTABLE_H_
#define TESTBED_ROUTINGTABLE_H_

#include "ARAMacros.h"
#include "Clock.h"
#include "Packet.h"
#include "Address.h"
#include "Testbed.h" 
#include "Environment.h"
#include "RoutingTable.h"
#include "NetworkInterface.h"
#include "EvaporationPolicy.h"
#include "ExponentialEvaporationPolicy.h"

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

        /**
         * @see RoutingTable::update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue);
         */
        void update(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface, float pheromoneValue);

        /**
         * @see RoutingTable::getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface)
         */
        float getPheromoneValue(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);

        /**
         * @see RoutingTable::update(AddressPtr destination, RoutingTableEntry* entry);
         */
        void update(AddressPtr destination, RoutingTableEntry* entry);

        /**
         * @see RoutingTable::removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface)
         */
        void removeEntry(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);
        
        /**
         * @see RoutingTable::isDeliverable(AddressPtr destination)
         */
        bool isDeliverable(AddressPtr destination);

        /**
         * @see RoutingTable::isDeliverable(const Packet* packet)
         */
        bool isDeliverable(const Packet* packet);

        /**
         * @see RoutingTable::exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface)
         */
        bool exists(AddressPtr destination, AddressPtr nextHop, NetworkInterface* interface);

        /**
         * @see RoutingTable::getPossibleNextHops(const Packet* packet)
         */
        RoutingTableEntryList getPossibleNextHops(const Packet* packet);

        /**
         * @see RoutingTable::getPossibleNextHops(AddressPtr destination)
         */
        RoutingTableEntryList getPossibleNextHops(AddressPtr destination);

        /**
         * @see RoutingTable::triggerEvaporation()
         */
        void triggerEvaporation();

        /**
         * @see RoutingTable::getTotalNumberOfEntries() const
         */
        unsigned int getTotalNumberOfEntries() const;

        /**
         * @see RoutingTable::getEntryAt(int position) const
         */
        RoutingTableEntryTupel getEntryAt(int position) const;

        /**
         * @see RoutingTable::getAllRoutesThatLeadOver(AddressPtr nextHop) const;
         */
        std::deque<RoutingTableEntryTupel> getAllRoutesThatLeadOver(AddressPtr nextHop) const;
    
        /**
         * @see RoutingTable::toString(int position=-1);
         */
        std::string toString(int position=-1);

    private:
        /**
         * This derived method does the same as the method in the base class, but uses
         * locks and mutexes in order to protect critical sections. The method is
         * called by the TestbedRoutingTable::triggerEvaporation() method. Unfortunately 
         * adding a mutex and calling RoutingTable::triggerEvaporation() did not work 
         * out (and so does a recursive mutex). However, we should come up with a
         * better solution which is not the the same code + plus mutex/lock
         *
         * FIXME: This method required a fix in terms of design (see comment above)
         */
        void applyEvaporation(Time* currentTime);

        /**
         * The mutex protects the routing table of class RoutingTable
         */
        mutable std::mutex mutex;
};

TESTBED_NAMESPACE_END

#endif /* TESTBED_ROUTINGTABLE_H_ */
