#include "LinearEvaporationPolicy.h"

#include <iostream>

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy():EvaporationPolicy(),q(0.1),threshold(0.2){ 

}

LinearEvaporationPolicy::~LinearEvaporationPolicy(){

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

