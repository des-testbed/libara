/*
 * $FU-Copyright$
 */

#ifndef EVAPORATION_POLICY_H_
#define EVAPORATION_POLICY_H_

#include <stdint.h>
#include <cstring>
#include <sys/time.h>

#include "TimeFactory.h"

namespace ARA { 

    /**
     * TODO write description
     */
    class EvaporationPolicy {
        public:
            EvaporationPolicy(unsigned int timeIntervalInMilliSeconds = 1000, unsigned int minimumTimeDifferenceBeforeEvaporation = 50) : timeInterval(timeIntervalInMilliSeconds), minimumTimeDifferenceBeforeEvaporation(minimumTimeDifferenceBeforeEvaporation) {}
            virtual ~EvaporationPolicy() {}

            /**
             * This method calculates a new evaporated pheromone value
             * based on an oldValue and when the last evaporation has taken place.
             */
            virtual float evaporate(float oldPheromoneValue, int milliSecondsSinceLastEvaporation) = 0;

            virtual bool isEvaporationNecessary(unsigned int timeDifferenceInMillis) {
                return timeDifferenceInMillis >= minimumTimeDifferenceBeforeEvaporation;
            }

            unsigned int getTimeInterval() const {
                return timeInterval;
            }

        protected:
            unsigned int timeInterval;
            unsigned int minimumTimeDifferenceBeforeEvaporation;
    };
} /* namespace ARA */

#endif 
