/*
 * $FU-Copyright$
 */

#ifndef OMNET_ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_
#define OMNET_ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_

#include "OMNeTARAMacros.h"
#include "EnergyAwareStochasticForwardingPolicy.h"

OMNETARA_NAMESPACE_BEGIN

/**
* The class provides a energy aware stochastic forwarding policy for class ARA. The
* class overwrites the getRandomNumber() method of the base class. It uses a
* pseudo-random number generator provided by OMNeT++.
*/
class OMNeTEnergyAwareStochasticForwardingPolicy : public cSimpleModule, public EnergyAwareStochasticForwardingPolicy {
    protected:
        /**
        *  The method returns a random number which uses OMNeT++ pseudo random number generators
        */
        float getRandomNumber();

        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

OMNETARA_NAMESPACE_END

#endif
