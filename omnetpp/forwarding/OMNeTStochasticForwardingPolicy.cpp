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

void OMNeTStochasticForwardingPolicy::handleMessage(cMessage* message) {
    throw cRuntimeError("handleMessage() should never be called on OMNeTStochasticForwardingPolicy!");
}

OMNETARA_NAMESPACE_END
