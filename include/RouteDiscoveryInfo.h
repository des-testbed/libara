/*
 * $FU-Copyright$
 */

#ifndef ROUTE_DISCOVERY_INFO_H_
#define ROUTE_DISCOVERY_INFO_H_

#include "ARAMacros.h"
#include "Packet.h"
#include "Context.h"

ARA_NAMESPACE_BEGIN

/**
 * A RouteDiscoveryInfo object holds data about a running route discovery.
 */
class RouteDiscoveryInfo : public Context {
    public:
        RouteDiscoveryInfo(const Packet* associatedPacket);
        virtual ~RouteDiscoveryInfo(){};

        virtual const Packet* getPacket();
        virtual int getNumberOfRetries();
        virtual void setNumberOfRetries(int newNumberOfRetries);

    private:
        int nrOfRetries;
        const Packet* originalPacket;
};

ARA_NAMESPACE_END

#endif // ROUTE_DISCOVERY_INFO_H_
