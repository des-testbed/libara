/*
 * $FU-Copyright$
 */

#include "BestPheromoneForwardingPolicy.h"
#include "Exception.h"

#include <deque>

ARA_NAMESPACE_BEGIN

BestPheromoneForwardingPolicy::BestPheromoneForwardingPolicy(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

NextHop* BestPheromoneForwardingPolicy::getNextHop(const Packet* packet) {
    RoutingTableEntryList possibleNextHops = routingTable->getPossibleNextHops(packet);
    if(possibleNextHops.empty()) {
        throw Exception("Could not determine next hop: there are no known routes to the destination");
    }

    RoutingTableEntry* bestEntry = nullptr;
    float globalMaximum = 0;
    for(auto& possibleNextHop: possibleNextHops) {
        if(possibleNextHop->getPheromoneValue() > globalMaximum) {
            bestEntry = possibleNextHop;
            globalMaximum = bestEntry->getPheromoneValue();
        }
    }

    return bestEntry->getNextHop();
}

ARA_NAMESPACE_END
