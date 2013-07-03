/*
 * $FU-Copyright$
 */

#include "ForwardingPolicy.h"

ARA_NAMESPACE_BEGIN

ForwardingPolicy::ForwardingPolicy(RoutingTable* routingTable) {
    this->routingTable = routingTable;
};

float ForwardingPolicy::calculateFitnessOfRoute(RoutingTableEntry* entry) const {
    return entry->getPheromoneValue();
}

ARA_NAMESPACE_END
