/*
 * $FU-Copyright$
 */

#include "LinearEvaporationPolicyMock.h"
#include "testAPI/mocks/time/TimeFactoryMock.h"

using namespace ARA;

LinearEvaporationPolicyMock::LinearEvaporationPolicyMock(float evaporationFactor, float threshold, int intervalInMilliSeconds) : LinearEvaporationPolicy(evaporationFactor, threshold, intervalInMilliSeconds) {
    // nothing to do here
}

float LinearEvaporationPolicyMock::getEvaporationFactor() const {
    return evaporationFactor;
}
