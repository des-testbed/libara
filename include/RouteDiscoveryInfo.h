/*
 * $FU-Copyright$
 */

#ifndef ROUTE_DISCOVERY_INFO_H_
#define ROUTE_DISCOVERY_INFO_H_

#include "ARAMacros.h"
#include "Packet.h"
#include "ContextObject.h"

ARA_NAMESPACE_BEGIN

/**
 * A RouteDiscoveryInfo object holds data about a running route discovery.
 */
class RouteDiscoveryInfo : public ContextObject {
    public:
        RouteDiscoveryInfo(const Packet* associatedPacket);

        const Packet* getPacket();
        int getNumberOfRetries();
        void setNumberOfRetries(int newNumberOfRetries);

    private:
        int nrOfRetries;
        const Packet* originalPacket;
};

ARA_NAMESPACE_END

#endif // ROUTE_DISCOVERY_INFO_H_
