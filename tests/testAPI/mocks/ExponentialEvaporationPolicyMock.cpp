/*
 * $FU-Copyright$
 */

#include "ExponentialEvaporationPolicyMock.h"

using namespace ARA;

ExponentialEvaporationPolicyMock::ExponentialEvaporationPolicyMock(float evaporationFactor, int intervalInMilliSeconds, float threshold) : ExponentialEvaporationPolicy(evaporationFactor, intervalInMilliSeconds, threshold) {
    // nothing to do here
}

float ExponentialEvaporationPolicyMock::getEvaporationFactor() const {
    return evaporationFactor;
}
