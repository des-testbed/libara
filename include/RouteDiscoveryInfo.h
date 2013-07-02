/*
 * $FU-Copyright$
 */

#ifndef ROUTE_DISCOVERY_INFO_H_
#define ROUTE_DISCOVERY_INFO_H_

#include "ARAMacros.h"
#include "Timer.h"

ARA_NAMESPACE_BEGIN

/**
 * A RouteDiscoveryInfo object holds data about a running route discovery.
 */
class RouteDiscoveryInfo {
    public:
        RouteDiscoveryInfo(const Packet* associatedPacket) {
            originalPacket = associatedPacket;
            nrOfRetries = 0;
        }

        int nrOfRetries;
        const Packet* originalPacket;
};

ARA_NAMESPACE_END

#endif // ROUTE_DISCOVERY_INFO_H_
