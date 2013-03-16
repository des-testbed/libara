/*
 * $FU-Copyright$
 */

#include "BestPheromoneForwardingPolicy.h"

#include <deque>

using namespace ARA;

// todo: add exception for "no hop available", are not yet interfaces are not yet considered
NextHop* BestPheromoneForwardingPolicy::getNextHop(const Packet* packet, RoutingTable* routingTable){
    std::deque<RoutingTableEntry*>* possibleNextHops = routingTable->getPossibleNextHops(packet);

    RoutingTableEntry* bestEntry = NULL;
    float globalMaximum = 0;
    for(auto& possibleNextHop: *possibleNextHops) {
        if(possibleNextHop->getPheromoneValue() > globalMaximum) {
            bestEntry = possibleNextHop;
            globalMaximum = bestEntry->getPheromoneValue();
        }
    }

    NextHop* result = bestEntry->getNextHop();
    //FIXME possibleNextHops muss noch gelöscht werden!
    return result;
}
