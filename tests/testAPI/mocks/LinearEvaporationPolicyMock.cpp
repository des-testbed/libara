#include "LinearEvaporationPolicyMock.h"
#include "testAPI/mocks/TimeFactoryMock.h"

using namespace ARA;

LinearEvaporationPolicyMock::LinearEvaporationPolicyMock(float evaporationFactor, long intervallInMilliSeconds) : EvaporationPolicy(new TimeFactoryMock()) {
    this->evaporationFactor = evaporationFactor;
    setInterval(intervallInMilliSeconds);
}

float LinearEvaporationPolicyMock::evaporate(float phi) {
    return evaporationFactor;
}

float LinearEvaporationPolicyMock::getEvaporationFactor() const {
    return evaporationFactor;
}
