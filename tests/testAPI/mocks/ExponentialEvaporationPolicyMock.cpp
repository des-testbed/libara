/*
 * $FU-Copyright$
 */

#include "ExponentialEvaporationPolicyMock.h"

using namespace ARA;

ExponentialEvaporationPolicyMock::ExponentialEvaporationPolicyMock(float evaporationFactor, float threshold, int intervalInMilliSeconds) : ExponentialEvaporationPolicy(evaporationFactor, threshold, intervalInMilliSeconds) {
    // nothing to do here
}

float ExponentialEvaporationPolicyMock::getEvaporationFactor() const {
    return evaporationFactor;
}
