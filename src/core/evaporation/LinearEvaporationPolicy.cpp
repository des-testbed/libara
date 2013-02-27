#include "LinearEvaporationPolicy.h"

#include <iostream>

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy(Time* last, Time* now, float pThreshold, float pQ) : EvaporationPolicy(last, now) {
    threshold = pThreshold;
    q = pQ;
}

/**
 * The methods provides the linear evaporation of pheromones as proposed
 * in the original ant routing algorithm (ARA). 
 * 
 * @param phi in The value of the pheromone which should be decreased
 *
 * @return The decreased pheromone value 
 */
float LinearEvaporationPolicy::evaporate(float phi){
   /// do the computation 
   phi = pow(((1 - this->q) * phi), this->factor); 

   /// check if the result is below a threshold
   if(phi < this->threshold){
     /// set pheromone to 0
     phi = 0;
   }
   return phi;
}

void LinearEvaporationPolicy::setThreshold(float threshold){
    this->threshold = threshold;
}

void LinearEvaporationPolicy::setLinearFactor(float factor){
    this->factor = factor;
}
