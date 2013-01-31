#include "LinearEvaporationPolicy.h"

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy():q(0.1),threshold(0.2){};

/**
 *
 * 
 */
void LinearEvaporationPolicy::evaporate(float *pheromone, uint8_t factor){
   /// do the computation 
   *pheromone = pow(((1 - this->q) * (*pheromone)), factor); 

   /// check if the result is below a threshold
   if(*pheromone < this->threshold){
     /// set pheromone to 0
     *pheromone = 0;
     /// throw exception
     throw NoRemainingPheromoneException("no pheromone left on route");
   }
}
