#include "CubicEvaporationPolicy.h"

using namespace ARA;

CubicEvaporationPolicy::CubicEvaporationPolicy():plateau(0.1),slow(0.2),reduction(0.3),threshold(0.2){};

/**
 *
 * 
 */
void CubicEvaporationPolicy::evaporate(float *pheromone, uint8_t factor){
    float t, m;
    // TODO: method ignores the factor right now :-(
    float a = 1 - (2 * (*pheromone));

    if(a > 0){
      t = 0.5 * (pow(abs(a), (1/this->plateau)) + 1);
    }else{
      t = 0.5 * (1 - pow(abs(a), (1/this->plateau)));
    }

    m = t + (this->reduction * this->slow);

    *pheromone = 0.5 * (pow(((2 * m) - 1), this->plateau) + 1);

    /// check if the result is below a threshold
    if(*pheromone < 0){
        /// set pheromone to 0
        *pheromone = 0;
        /// throw exception
        throw NoRemainingPheromoneException("no pheromone left on route");
    }

    if(*pheromone > 1){
        // TODO: check!
        *pheromone = 1;
    }
}
