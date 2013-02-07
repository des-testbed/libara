#include "LinearEvaporationPolicy.h"

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy():EvaporationPolicy(),threshold(0.2),q(0.1){ 

}

LinearEvaporationPolicy::LinearEvaporationPolicy(float pThreshold, float pQ):EvaporationPolicy(),threshold(pThreshold),q(pQ){ 

}

/**
 *
 * 
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

