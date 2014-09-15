/*
 * $FU-Copyright$
 */

#ifndef TESTBED_ROUTE_DISCOVERY_INFO_H_
#define TESTBED_ROUTE_DISCOVERY_INFO_H_

#include "Testbed.h"
#include "TestbedPacket.h"
#include "RouteDiscoveryInfo.h"

#include <mutex>

TESTBED_NAMESPACE_BEGIN

/**
 * A TestbedRouteDiscoveryInfo object holds data about a running route discovery.
 */
class TestbedRouteDiscoveryInfo : public RouteDiscoveryInfo {
    public:
        TestbedRouteDiscoveryInfo(const Packet* associatedPacket);

        const TestbedPacket* getPacket();
        int getNumberOfRetries();
        void setNumberOfRetries(int newNumberOfRetries);

    private:
        std::recursive_mutex packetMutex;
};

TESTBED_NAMESPACE_END

#endif // TESTBED_ROUTE_DISCOVERY_INFO_H_
