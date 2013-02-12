#include "CubicEvaporationPolicy.h"

using namespace ARA;

CubicEvaporationPolicy::CubicEvaporationPolicy():EvaporationPolicy(),plateau(3),slow(0.2),reduction(0.3),threshold(0.2){ }

CubicEvaporationPolicy::CubicEvaporationPolicy(int pPlateau, float pSlow, float pReduction, float pThreshold)
:EvaporationPolicy(),plateau(pPlateau),slow(pSlow),reduction(pReduction),threshold(pThreshold){ }

CubicEvaporationPolicy::CubicEvaporationPolicy(Time *a, Time *b)
:EvaporationPolicy(a, b),plateau(3),slow(0.2),reduction(0.3),threshold(0.2){ }

CubicEvaporationPolicy::~CubicEvaporationPolicy(){ }

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
float CubicEvaporationPolicy::evaporate(float pheromone){
    /// we iterate 'factor' times since it is not (yet) possible just to use the pow function
    for(int i = 0; i < this->factor; i++){
        float t, m;
        float a = 1 - (2 * (pheromone));

        if(a > 0){
            t = 0.5 * (pow(abs(a), (1/this->plateau)) + 1);
        }else{
            t = 0.5 * (1 - pow(abs(a), (1/this->plateau)));
        }

        m = t + (this->reduction * this->slow);

        pheromone = 0.5 * (pow(((2 * m) - 1), this->plateau) + 1);

        /// check if the result is below a threshold
        if(pheromone < 0){
            /// set pheromone to 0
            return 0;
        }

        if(pheromone > 1){
            // TODO: check!
            pheromone = 1;
        }
    }

    return pheromone;
}
