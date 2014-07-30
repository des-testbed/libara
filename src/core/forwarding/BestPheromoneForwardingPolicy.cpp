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
    RoutingTableEntry* bestEntry = nullptr;

    if (possibleNextHops.empty()) {
        throw Exception("Could not determine next hop: there are no known routes to the destination");
    } else {
        float globalMaximum = 0;

        for (auto& possibleNextHop: possibleNextHops) {
            if (possibleNextHop->getPheromoneValue() > globalMaximum) {
                bestEntry = possibleNextHop;
                globalMaximum = bestEntry->getPheromoneValue();
            }
        }

        // this should never happen, but nobody should say never
        if ((globalMaximum == 0) || bestEntry == nullptr) {
            return nullptr;
        }

        return bestEntry->getNextHop();
    }

    return nullptr;
}

ARA_NAMESPACE_END
