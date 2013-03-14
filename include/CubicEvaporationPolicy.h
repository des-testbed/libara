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
    class CubicEvaporationPolicy : public EvaporationPolicy {
        public:
            CubicEvaporationPolicy(int pPlateau, float pSlow, float pReduction, float pThreshold, unsigned int timeIntervalInMilliSeconds = 1000);

            float evaporate(float oldPheromoneValue, int milliSecondsSinceLastEvaporation);

            void setPlateau(float plateau);
            void setSlow(float slow);
            void setReduction(float reduction);
            void setThreshold(float threshold);

        private:
            float plateau;
            float slow;
            float reduction;
            float threshold;
    };
} /* namespace ARA */

#endif 
