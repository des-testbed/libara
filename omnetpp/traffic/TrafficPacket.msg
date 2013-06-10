cplusplus {{
#include "OMNeTARAMacros.h"
#include <deque>
typedef std::deque<ARA::AddressPtr> Route;
}}

class noncobject Route;

namespace ARA::omnetpp;

packet TrafficPacket {
    simtime_t creationTime;
    Route route;
    int routeEnergy = 0;
};
