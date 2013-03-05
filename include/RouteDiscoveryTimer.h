/*
 * $FU-Copyright$
 */


#ifndef ROUTE_DISCOVERY_TIMER_H_
#define ROUTE_DISCOVERY_TIMER_H_

#include "Address.h"

#include <unordered_map>

namespace ARA {
    /**
     * The class provides a interface for the management of route discovery timers in the ARA. For
     * every route discovery a timer is set which defines the timespan until a route for a given 
     * destination should be established. Typically, a number of retries is defined until the data
     * packets for a destination are discarded.
     */
    class RouteDiscoveryTimer {
        public:
            virtual ~RouteDiscoveryTimer();

            /// the method checks if a route discovery for the specified address is running
            virtual bool isRunning(std::shared_ptr<Address> address);
            /// the method initializes a route discovery timer
            virtual void addRouteDiscoveryTimer(std::shared_ptr<Address> address, int retries);
            /// the method decreases the number of retry attempts for the given address
            virtual void decreaseRetries(std::shared_ptr<Address> address);

        private:
            /// the map (address => 
            std::unordered_map<std::shared_ptr<Address>, int, AddressHash, AddressPredicate> table;
    };
}

#endif
