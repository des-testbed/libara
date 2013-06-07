/*
 * $FU-Copyright$
 */

#include "CubicEvaporationPolicy.h"
#include "Exception.h"

#include <cmath>

using namespace ARA;

CubicEvaporationPolicy::CubicEvaporationPolicy(unsigned int millisecondsUntilPlateauCenterIsReached, float plateauCenterDepth, unsigned int oddExponent) {
    checkExponent(oddExponent);

    this->alpha = millisecondsUntilPlateauCenterIsReached;
    this->plateauCenterDepth = plateauCenterDepth;
    this->exponent = oddExponent;
}

void CubicEvaporationPolicy::checkExponent(unsigned int exponent) {
    if(exponent % 2 == 0) {
        throw Exception("The exponent of the cubic evaporation function can not be even");
    }
}

float CubicEvaporationPolicy::evaporate(float oldPheromoneValue, float milliSecondsSinceLastTraffic) {
    float potentiatedAlpha = pow(alpha, exponent);
    float factor = plateauCenterDepth / potentiatedAlpha;
    float amountOfEvaporation = factor * (potentiatedAlpha + pow(milliSecondsSinceLastTraffic - alpha, exponent));
    float newPheromoneValue = oldPheromoneValue - amountOfEvaporation;

    if(newPheromoneValue < 0) {
        newPheromoneValue = 0;
    }

    return newPheromoneValue;
}

void CubicEvaporationPolicy::setTimeUntilPlateauCenterIsReached(unsigned int millisecondsUntilPlateauCenterIsReached) {
    alpha = millisecondsUntilPlateauCenterIsReached;
}

void CubicEvaporationPolicy::setPlateauCenterDepth(float newPlateauCenterDepth) {
    plateauCenterDepth = newPlateauCenterDepth;
}

void CubicEvaporationPolicy::setExponent(unsigned int newOddExponent) {
    checkExponent(newOddExponent);
    exponent = newOddExponent;
}
