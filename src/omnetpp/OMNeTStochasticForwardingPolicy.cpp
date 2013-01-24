#include "OMNeTStochasticForwardingPolicy.h"

using namespace ARA;

OMNeTStochasticForwardingPolicy::OMNeTStochasticForwardingPolicy(RoutingTable* routingTable) : 
 StochasticForwardingPolicy(routingTable) { }

float OMNeTStochasticForwardingPolicy::getRandomNumber(){
   return dblrand();
}
