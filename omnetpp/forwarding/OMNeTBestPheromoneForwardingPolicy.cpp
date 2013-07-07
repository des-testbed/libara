/*
 * $FU-Copyright$
 */

#include "OMNeTBestPheromoneForwardingPolicy.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(OMNeTBestPheromoneForwardingPolicy);

void OMNeTBestPheromoneForwardingPolicy::setRoutingTable(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

OMNETARA_NAMESPACE_END
