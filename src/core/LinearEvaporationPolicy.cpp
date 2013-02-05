#include "LinearEvaporationPolicy.h"

#include <iostream>

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy():q(0.1),threshold(0.2),factor(1),interval(1){ 
    this->lastAccessTime = new timeval;
    /// initialize the last access time with '0'
    memset(this->lastAccessTime, 0, sizeof(timeval));
}

LinearEvaporationPolicy::~LinearEvaporationPolicy(){
    delete this->lastAccessTime;
} 

/**
 * The method checks if a given threshold is passed time-wise. If it is so, 
 * a factor is determined which denotes how often the evaporate method should
 * be called, e.g. assuming that the evaporation method should be called
 * every second and if 30 seconds have passed since the last access to the routing
 * table, the evaporation method has to be called 30 times. 
 *
 * @return The method returns true if the evaporation method should be trigged, 
 *   false otherwise
 */
bool LinearEvaporationPolicy::checkForEvaporation(){
    /// check if the last access time to the routing tables has been initialized
    if(this->lastAccessTime->tv_sec != 0){
        struct timeval *now = new timeval;
        /// get the current time
        gettimeofday(now, 0);

        /// FIXME (it's not so cool to create all the time an object)
        Time* time = new Time();
        /// compute the time difference in seconds
        int timeDifference = time->getTimeDifferenceInMilliseconds(now, lastAccessTime);
        /// delete the object
        delete time;

        /// compare the timestamps 
        if(timeDifference > this->interval){
            /// compute the factor
            determineEvaporationFactor(timeDifference);
            /// delete the last access time
            delete this->lastAccessTime;
            /// update the timestamp
            this->lastAccessTime = now;
            /// set the factor

            /// return the result
            return true;
        }

        /// delete the last access time
        delete this->lastAccessTime;
        /// update the timestamp
        this->lastAccessTime = now;
    }else{
        /// initialize the last access time
        gettimeofday(this->lastAccessTime, 0);
    } 
    return false;
}

void LinearEvaporationPolicy::setInterval(int interval){
    this->interval = interval;
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

/** 
 *
 */
void LinearEvaporationPolicy::determineEvaporationFactor(float timeDifference){
    /// every 10 ms
    this->factor = ((uint8_t)timeDifference) %  100;
}
