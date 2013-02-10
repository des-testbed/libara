#include "OMNeTStochasticForwardingPolicy.h"

namespace ARA {
namespace omnetpp {

OMNeTStochasticForwardingPolicy::OMNeTStochasticForwardingPolicy(RoutingTable* routingTable) : 
 StochasticForwardingPolicy(routingTable) { }

float OMNeTStochasticForwardingPolicy::getRandomNumber(){
   return dblrand();
}

} /* namespace omnetpp */
} /* namespace ARA */
