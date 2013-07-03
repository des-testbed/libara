/*
 * $FU-Copyright$
 */

#include "OMNeTBestPheromoneForwardingPolicy.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(OMNeTBestPheromoneForwardingPolicy);

void OMNeTBestPheromoneForwardingPolicy::setRoutingTable(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

void OMNeTBestPheromoneForwardingPolicy::handleMessage(cMessage* message) {
    throw cRuntimeError("handleMessage() should never be called on OMNeTBestPheromoneForwardingPolicy!");
}

OMNETARA_NAMESPACE_END
