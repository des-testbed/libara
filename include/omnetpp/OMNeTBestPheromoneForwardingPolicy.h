/*
 * $FU-Copyright$
 */

#ifndef OMNET_BEST_FORWARDING_POLICY_H_
#define OMNET_BEST_FORWARDING_POLICY_H_

#include "OMNeTARAMacros.h"
#include "BestPheromoneForwardingPolicy.h"
#include "OMNeTForwardingPolicy.h"
#include "RoutingTable.h"

OMNETARA_NAMESPACE_BEGIN

/**
* The class provides a forwarding policy which selects the next hop with the highest pheromone value.
*
* OMNeT++ does only support the standard constructor for its SimpleModules.
* This class implements that and adds a setRoutingTable method which
* MUST BE CALLED before this object is used.
*/
class OMNeTBestPheromoneForwardingPolicy : public BestPheromoneForwardingPolicy, public OMNeTForwardingPolicy {
    public:
        OMNeTBestPheromoneForwardingPolicy() : BestPheromoneForwardingPolicy(nullptr) {};

        /**
         * This must be called after the constructor, because stupid OMNeT++ does only allow standard constructors...
         */
        void setRoutingTable(RoutingTable* routingTable);
};

OMNETARA_NAMESPACE_END

#endif
