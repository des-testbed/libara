/*
 * $FU-Copyright$
 */

#ifndef OMNET_BEST_PHEROMONE_STOCHASTIC_FORWARDING_POLICY_H_
#define OMNET_BEST_PHEROMONE_STOCHASTIC_FORWARDING_POLICY_H_

#include <numeric>
#include <omnetpp.h>
#include <csimplemodule.h>

#include "RoutingTable.h"
#include "ForwardingPolicy.h"
#include "BestPheromoneForwardingPolicy.h"

namespace ARA {
    namespace omnetpp {
           /**
            * The class provides a forwarding policy which selects the next hop with the highest pheromone value. 
            */
           class OMNeTBestPheromoneForwardingPolicy : public cSimpleModule, public BestPheromoneForwardingPolicy {
              protected:
                 virtual void initialize();
                 virtual void handleMessage(cMessage *msg);
           };
    } /* namespace omnetpp */
} /* namespace ARA */

#endif
