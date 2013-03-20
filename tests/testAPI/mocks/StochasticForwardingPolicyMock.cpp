/*
 * $FU-Copyright$
 */

#include "StochasticForwardingPolicyMock.h"

using namespace ARA;

StochasticForwardingPolicyMock::StochasticForwardingPolicyMock(unsigned int seed) {
   this->initializeRandomNumberGenerator(seed);
}

