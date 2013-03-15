/*
 * $FU-Copyright$
 */

#ifndef OMNET_ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_
#define OMNET_ENERGY_AWARE_STOCHASTIC_FORWARDING_POLICY_H_

#include <numeric>
#include <omnetpp.h>
#include <csimplemodule.h>

#include "RoutingTable.h"
#include "ForwardingPolicy.h"
#include "EnergyAwareStochasticForwardingPolicy.h"

namespace ARA {
    namespace omnetpp {
           /**
            * The class provides a energy aware stochastic forwarding policy for class ARA. The
            * class overwrites the getRandomNumber() method of the base class. It uses a
            * pseudo-random number generator provided by OMNeT++.
            */
           class OMNeTEnergyAwareStochasticForwardingPolicy : public cSimpleModule, public EnergyAwareStochasticForwardingPolicy {
              protected:
                 /// The method returns a random number wich uses OMNeT++ pseudo random number generators
                 float getRandomNumber();

                 virtual void initialize();
                 virtual void handleMessage(cMessage *msg);
           };

    } /* namespace omnetpp */
} /* namespace ARA */

#endif
