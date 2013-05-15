/*
 * $FU-Copyright$
 */

#include "StochasticForwardingPolicyMock.h"
#include <ctime>

using namespace ARA;

StochasticForwardingPolicyMock::StochasticForwardingPolicyMock() {
    this->initializeRandomNumberGenerator(time(0));
}

StochasticForwardingPolicyMock::StochasticForwardingPolicyMock(unsigned int seed) {
    this->initializeRandomNumberGenerator(seed);
}

