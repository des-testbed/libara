#include "LinearEvaporationPolicy.h"

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy():q(0.1),threshold(0.2){};

bool LinearEvaporationPolicy::checkForEvaporation(){
    return false;
}
/**
 *
 * 
 */
float LinearEvaporationPolicy::evaporate(){
   // FIXME
   float pheromone = 5.0;
   // FIXME
   int factor = 1;

   /// do the computation 
   pheromone = pow(((1 - this->q) * pheromone), factor); 

   /// check if the result is below a threshold
   if(pheromone < this->threshold){
     /// set pheromone to 0
     pheromone = 0;
   }
   return pheromone;
}
