/*
 * $FU-Copyright$
 */

#include "LinearEvaporationPolicy.h"

#include <cmath>
#include <iostream>

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy(float evaporationFactor, float threshold, unsigned int timeIntervalInMilliSeconds) : EvaporationPolicy(timeIntervalInMilliSeconds) {
    this->evaporationFactor = evaporationFactor;
    this->threshold = threshold;
}

/**
 * The method evaporates pheromones in a linear fashion. This means that pheromones evaporate
 * as follows:
 *
 *   f(t)= -m * t + phi
 *
 * Here, m denotes the evaporation factor, meaning how fast pheromones evaporate and t the 
 * corresponding point in time. However, a master thesis which optimizes the testbed version of
 * the algorithm states that if less than 3 next hops to the destination are known, the evaporation
 * is slowed down by an additional factor.
 *
 * TODO: take the next hops into account
 */
float LinearEvaporationPolicy::evaporate(float oldPheromoneValue, int millisecondsSinceLastEvaporation){
    if(millisecondsSinceLastEvaporation == 0) {
        return oldPheromoneValue;
    } else {
        float multiplicator = ((float)millisecondsSinceLastEvaporation) / ((float)timeInterval);
        /// this corresponds to f(x) = -mx + b or f(multiplicator) = -1 * evaporationfactor * multiplicator + oldPheromoneValue
        float newPheromoneValue = ((-1) * this->evaporationFactor * multiplicator) + oldPheromoneValue;

        if (newPheromoneValue < threshold) {
            newPheromoneValue = 0;
        }

        return newPheromoneValue;
    }
}
