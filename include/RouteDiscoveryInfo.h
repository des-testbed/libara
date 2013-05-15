/*
 * $FU-Copyright$
 */

#ifndef ROUTE_DISCOVERY_INFO_H_
#define ROUTE_DISCOVERY_INFO_H_

#include "Timer.h"

namespace ARA {

/**
 * A RouteDiscoveryInfo object holds data about a running route discovery.
 */
struct RouteDiscoveryInfo {
    int nrOfRetries;
    Timer* timer;
    const Packet* originalPacket;
};

} /* namespace ARA */
#endif // ROUTE_DISCOVERY_INFO_H_
