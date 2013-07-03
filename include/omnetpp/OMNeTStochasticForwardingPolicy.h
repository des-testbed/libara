/*
 * $FU-Copyright$
 */

#ifndef OMNET_STOCHASTIC_FORWARDING_POLICY_H_
#define OMNET_STOCHASTIC_FORWARDING_POLICY_H_

#include "OMNeTARAMacros.h"
#include "StochasticForwardingPolicy.h"
#include "OMNeTForwardingPolicy.h"
#include "RoutingTable.h"

OMNETARA_NAMESPACE_BEGIN

/**
* The class provides a stochastic forwarding policy for class OMNeTARA. The
* class overwrites the getRandomNumber() method of the base class. It uses a
* pseudo-random number generator provided by OMNeT++.
*
* OMNeT++ does only support the standard constructor for its SimpleModules.
* This class implements that and adds a setRoutingTable method which
* MUST BE CALLED before this object is used.
*/
class OMNeTStochasticForwardingPolicy : public cSimpleModule, public StochasticForwardingPolicy, public OMNeTForwardingPolicy {
    public:
        OMNeTStochasticForwardingPolicy() : StochasticForwardingPolicy(nullptr) {};

        /**
         * This must be called after the constructor, because stupid OMNeT++ does only allow standard constructors...
         */
        void setRoutingTable(RoutingTable* routingTable);

    protected:
        /**
         * Returns a random number which uses OMNeT++ pseudo random number generators.
         */
        virtual float getRandomNumber();
        virtual void handleMessage(cMessage *msg);
};

OMNETARA_NAMESPACE_END

#endif
