/*
 * $FU-Copyright$
 */

#ifndef LINEAR_EVAPORATION_POLICY_H_
#define LINEAR_EVAPORATION_POLICY_H_

#include "EvaporationPolicy.h"
#include "Clock.h"

namespace ARA {

   /**
    * This class provides the linear evaporation function of the ant routing algorithm (ARA).
    */
    class LinearEvaporationPolicy : public EvaporationPolicy {
        public:

            /**
             * Creates a new LinearEvaporationPolicy with the given Clock.
             *
             * @param evaporationFactor the factor which is used to evaporate the pheromone values
             * @param threshold denotes at what point the pheromone level is set to 0
             * @param timeIntervalMillis denotes the time interval in milliseconds at which the evaporation periodically should take place
             */
            LinearEvaporationPolicy(float evaporationFactor, float threshold, unsigned int timeIntervalMillis = 1000);

            float evaporate(float oldPheromoneValue, int millisecondsSinceLastEvaporation);

        protected:
            float threshold;
            float evaporationFactor;
    };
} /* namespace ARA */

#endif 
