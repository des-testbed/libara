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
    *
    */
    class EvaporationPolicy {
        public:
            EvaporationPolicy(TimeFactory* timeFactory);
            virtual ~EvaporationPolicy();

            /// the method checks how much time has passed since the last access to the routing table
            bool checkForEvaporation();
            /// sets the time until the evaporation is triggered
            void setInterval(int interval);
            /// the method returns how often the evaporation should take place
            int getFactor();

            /// the method reduces the pheromone value of a routing table entry
            virtual float evaporate(float phi) = 0;

        protected:
            /// the factor which indicates how often the evaporation should take place 
            uint8_t factor;

        private:
            bool tableHasBeenAccessedEarlier();

            void determineEvaporationFactor(int timeDifference);

            /**
             * The TimeFactory that is used to get the concrete Time implementations.
             */
            TimeFactory* timeFactory;

            /// the last access time of the routing table
            Time *lastAccessTime;

            /// the interval which denotes how much time has to pass in order to trigger the evaporation
            long int interval;
    };
} /* namespace ARA */

#endif 
