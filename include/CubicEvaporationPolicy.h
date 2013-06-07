/*
 * $FU-Copyright$
 */

#ifndef CUBIC_EVAPORATION_POLICY_H_
#define CUBIC_EVAPORATION_POLICY_H_

#include "ARAMacros.h"

ARA_NAMESPACE_BEGIN

/**
* This class provides the cubic evaporation function for an ant routing algorithm.
* The formula has been taken from the Masters Thesis "Optimization and evaluation
* of energy aware ant routing algorithm strategies based on network simulations"
* by Friedrich Große.
*
* Even though this is called cubic evaporation it will work also with odd exponents greater
* than three.
*
* The general formula is the following:
*
*     f(Tau) = P/(alpha^b) * ( alpha^b + (Tau - alpha)^b )
*
*     Tau = the time since the last traffic has been sent.
*     P = the height of the plateau. This corresponds to the amount of pheromone which will
*         later be subtracted from the last known pheromone value when traffic has been sent
*     alpha = the time since the center of the plateau should be reached
*     b = an odd integer exponent greater than zero
*/
class CubicEvaporationPolicy {
    public:
        CubicEvaporationPolicy(unsigned int millisecondsUntilPlateauCenterIsReached, float plateauCenterDepth, unsigned int oddExponent);
        float evaporate(float oldPheromoneValue, float milliSecondsSinceLastTraffic);

        // note that the setters are neccessary for OMNeT++ simulations because we can't use any constructor other than the default one.

        void setTimeUntilPlateauCenterIsReached(unsigned int millisecondsUntilPlateauCenterIsReached);
        void setPlateauCenterDepth(float newPlateauCenterDepth);
        void setExponent(unsigned int newOddExponent);

    protected:
        void checkExponent(unsigned int exponent);

    private:
        unsigned int alpha;
        float plateauCenterDepth;
        unsigned int exponent;
};

ARA_NAMESPACE_END

#endif
