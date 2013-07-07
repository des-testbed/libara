/*
 * $FU-Copyright$
 */

#include "OMNeTStochasticForwardingPolicy.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(OMNeTStochasticForwardingPolicy);

float OMNeTStochasticForwardingPolicy::getRandomNumber(){
   return dblrand();
}

void OMNeTStochasticForwardingPolicy::setRoutingTable(RoutingTable* routingTable) {
    this->routingTable = routingTable;
}

OMNETARA_NAMESPACE_END
