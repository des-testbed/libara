#include "CubicEvaporationPolicy.h"

using namespace ARA;

CubicEvaporationPolicy::CubicEvaporationPolicy():EvaporationPolicy(),plateau(0.1),slow(0.2),reduction(0.3),threshold(0.2){

}

CubicEvaporationPolicy::~CubicEvaporationPolicy(){

}

/**
 *
 * 
 */
float CubicEvaporationPolicy::evaporate(float phi){
   // FIXME
   float pheromone = 5.0;
   // FIXME
   int factor = 1;
    float t, m;
    // TODO: method ignores the factor right now :-(
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
        pheromone = 0;
    }

    if(pheromone > 1){
        // TODO: check!
        pheromone = 1;
    }

    return pheromone;
}
