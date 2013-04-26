/*
 * $FU-Copyright$
 */

#ifndef EXPONENTIAL_EVAPORATION_POLICY_H_
#define EXPONENTIAL_EVAPORATION_POLICY_H_

#include "ARAMacros.h"
#include "EvaporationPolicy.h"

ARA_NAMESPACE_BEGIN

/**
* This class provides the exponential evaporation function of the ant routing algorithm (ARA).
*/
class ExponentialEvaporationPolicy : public EvaporationPolicy {
    public:

        /**
         * Creates a new ExponentialEvaporationPolicy
         *
         * The pheromones will be evaporated every x milliseconds (time interval)
         * according to the following formula: newPhi = oldPhi * factor
         *
         * @param evaporationFactor the factor which is used to evaporate the pheromone values
         * @param timeIntervalMillis denotes the time interval in milliseconds at which the evaporation factor should be applied
         * @param threshold denotes the smallest allowed pheromone value before it is set to 0
         */
        ExponentialEvaporationPolicy(float evaporationFactor, unsigned int timeIntervalMillis, float threshold);

        float evaporate(float oldPheromoneValue, int millisecondsSinceLastEvaporation);

    protected:
        float threshold;
        float evaporationFactor;
};

ARA_NAMESPACE_END

#endif 
