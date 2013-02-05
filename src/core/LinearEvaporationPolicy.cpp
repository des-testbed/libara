#include "LinearEvaporationPolicy.h"

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy():q(0.1),threshold(0.2),factor(1),interval(1.0){ 
    this->lastAccessTime = new timeval;
    /// initialize the last access time with '0'
    memset(this->lastAccessTime, 0, sizeof(timeval));
}

LinearEvaporationPolicy::~LinearEvaporationPolicy(){
    delete this->lastAccessTime;
} 

bool LinearEvaporationPolicy::checkForEvaporation(){
    /// check if the last access time to the routing tables has been initialized
    if(this->lastAccessTime->tv_sec != 0){
        struct timeval *now = new timeval;
        /// get the current time
        gettimeofday(now, 0);

        /// FIXME (it's not so cool to create all the time an object)
        Time* time = new Time();
        /// compute the time difference in seconds
        float timeDifference = time->getTimeDifferenceInSeconds(now, lastAccessTime);
        /// delete the object
        delete time;

        /// compare the timestamps 
        if(timeDifference > this->interval){
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

void LinearEvaporationPolicy::setInterval(float interval){
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
