/*
 * $FU-Copyright$
 */

#ifndef PANT_TIMER_INFO_H_
#define PANT_TIMER_INFO_H_

#include "ARAMacros.h"

ARA_NAMESPACE_BEGIN

/**
 * A RouteDiscoveryInfo object holds data about a scheduled PANT
 */
class PantTimerInfo {
    public:
        PantTimerInfo(AddressPtr destination) {
            this->destination = destination;
        }

        AddressPtr destination;
};

ARA_NAMESPACE_END

#endif // ROUTE_DISCOVERY_INFO_H_
