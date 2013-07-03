/*
 * $FU-Copyright$
 */

#ifndef OMNET_ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_
#define OMNET_ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_

#include "OMNeTARAMacros.h"
#include "EnergyAwareStochasticForwardingPolicy.h"
#include "OMNeTEARAForwardingPolicy.h"
#include "EnergyAwareRoutingTable.h"

OMNETARA_NAMESPACE_BEGIN

/**
* The class provides a energy aware stochastic forwarding policy for class ARA. The
* class overwrites the getRandomNumber() method of the base class. It uses a
* pseudo-random number generator provided by OMNeT++.
*
* OMNeT++ does only support the standard constructor for its SimpleModules.
* This class implements that and adds a setRoutingTable method which
* MUST BE CALLED before this object is used.
*/
class OMNeTEnergyAwareStochasticForwardingPolicy : public cSimpleModule, public EnergyAwareStochasticForwardingPolicy, public OMNeTEARAForwardingPolicy {
    public:
        OMNeTEnergyAwareStochasticForwardingPolicy() : EnergyAwareStochasticForwardingPolicy(nullptr) {};

        /**
         * This must be called after the constructor, because stupid OMNeT++ does only allow standard constructors...
         */
        void setRoutingTable(EnergyAwareRoutingTable* routingTable);
    protected:
        /**
        *  The method returns a random number which uses OMNeT++ pseudo random number generators
        */
        float getRandomNumber();

        virtual void initialize();
        virtual void handleMessage(cMessage* message);
};

OMNETARA_NAMESPACE_END

#endif
