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
    std::deque<RoutingTableEntry*> possibleNextHops = routingTable->getPossibleNextHops(packet);
    AddressPtr sender = packet->getSender();

    RoutingTableEntry* bestEntry = nullptr;
    float globalMaximum = 0;
    for(auto& possibleNextHop: possibleNextHops) {
        if(possibleNextHop->getAddress()->equals(sender)== false) {
            if(possibleNextHop->getPheromoneValue() > globalMaximum) {
                bestEntry = possibleNextHop;
                globalMaximum = bestEntry->getPheromoneValue();
            }
        }
    }

    if(bestEntry == nullptr) {
        if(possibleNextHops.empty()) {
            throw Exception("Could not determine next hop: there are no known routes to the destination");
        }
        else {
            throw Exception("Could not determine next hop: only possible route to destination leads back to the sender");
        }
    }
    else {
        return bestEntry->getNextHop();
    }
}

ARA_NAMESPACE_END
