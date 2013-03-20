/*
 * $FU-Copyright$
 */

#include "EnergyAwareStochasticForwardingPolicyMock.h"

using namespace ARA;

EnergyAwareStochasticForwardingPolicyMock::EnergyAwareStochasticForwardingPolicyMock(unsigned int seed) {
   this->initializeRandomNumberGenerator(seed);
}

