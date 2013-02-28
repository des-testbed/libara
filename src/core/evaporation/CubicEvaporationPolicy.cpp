#include "CubicEvaporationPolicy.h"

#include <cmath>
#include <cstdlib>
#include <stdint.h>

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

/**
 * FIXME: There is something wrong 
 * 
 */
float CubicEvaporationPolicy::evaporate(float oldPheromoneValue, int milliSecondsSinceLastEvaporation) {
    if(milliSecondsSinceLastEvaporation == 0) {
        return oldPheromoneValue;
    }
    else {
        int factor = milliSecondsSinceLastEvaporation / timeInterval;
        float newPheromone;

        /// we iterate 'factor' times since it is not (yet) possible just to use the pow function
        for(int i = 0; i < factor; i++){
            float t, m;
            float a = 1 - (2 * oldPheromoneValue);

            if(a > 0){
                t = 0.5 * (pow(abs(a), (1/this->plateau)) + 1);
            }else{
                t = 0.5 * (1 - pow(abs(a), (1/this->plateau)));
            }

            m = t + (this->reduction * this->slow);

            newPheromone = 0.5 * (pow(((2 * m) - 1), this->plateau) + 1);

            /// check if the result is below a threshold
            if(newPheromone < 0){
                /// set pheromone to 0
                return 0;
            }

            if(newPheromone > 1){
                // TODO: check!
                newPheromone = 1;
            }
        }

        return newPheromone;
    }
}
