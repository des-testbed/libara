/*
 * $FU-Copyright$
 */

#include <cmath>
#include <cstdlib>
#include <stdint.h>

#include "CubicEvaporationPolicy.h"

using namespace ARA;

CubicEvaporationPolicy::CubicEvaporationPolicy(int pPlateau, float pSlow, float pReduction, float pThreshold, unsigned int timeIntervalInMilliSeconds) : EvaporationPolicy(timeIntervalInMilliSeconds) {
    this->plateau = pPlateau;
    this->slow = pSlow;
    this->reduction = pReduction;
    this->threshold = pThreshold;
}

void CubicEvaporationPolicy::setPlateau(float pPlateau){
    this->plateau = pPlateau;
}

void CubicEvaporationPolicy::setSlow(float pSlow){
    this->slow = pSlow;
}

void CubicEvaporationPolicy::setReduction(float pReduction){
    this->reduction = pReduction;
}

void CubicEvaporationPolicy::setThreshold(float pThreshold){
    this->threshold = pThreshold;
}

float CubicEvaporationPolicy::evaporate(float oldPheromoneValue, int milliSecondsSinceLastEvaporation) {
    if(milliSecondsSinceLastEvaporation == 0) {
        return oldPheromoneValue;
    }
    else {
	    float newPheromone = oldPheromoneValue;
            newPheromone = slow*(1.0+threshold-pow(((milliSecondsSinceLastEvaporation-2000)/reduction),3)/plateau);


            /// check if the result is below a threshold
            if(newPheromone < 0){
                /// set pheromone to 0
                return 0;
            }

            if(newPheromone > 1){
                // TODO: check!
                newPheromone = 1;
            }

        return newPheromone;
    }
}
