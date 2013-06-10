cplusplus {{
#include "OMNeTARAMacros.h"
#include <deque>
typedef std::deque<ARA::AddressPtr> Route;
typedef std::deque<unsigned char> RouteEnergy;
}}

class noncobject Route;
class noncobject RouteEnergy;

namespace ARA::omnetpp;

packet TrafficPacket {
    simtime_t creationTime;
    Route route;
    RouteEnergy routeEnergy;
};
