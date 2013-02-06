#include "EvaporationPolicy.h"

#include <iostream>

using namespace ARA;

EvaporationPolicy::EvaporationPolicy():factor(1),interval(100){ 
    this->lastAccessTime = new timeval;
    /// initialize the last access time with '0'
    memset(this->lastAccessTime, 0, sizeof(timeval));
    this->time = new Time();
}

EvaporationPolicy::~EvaporationPolicy(){
    delete this->time;
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
bool EvaporationPolicy::checkForEvaporation(){
    /// check if the last access time to the routing tables has been initialized
    if(this->lastAccessTime->tv_sec != 0){
        struct timeval *now = new timeval;
        /// get the current time
        gettimeofday(now, 0);

        int timeDifference = this->getTimeDifference(now);
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

/** 
 *
 */
void EvaporationPolicy::determineEvaporationFactor(int timeDifference){
    this->factor = (uint8_t)(timeDifference/this->interval);
}

void EvaporationPolicy::setInterval(int interval){
    this->interval = interval;
}

int EvaporationPolicy::getTimeDifference(struct timeval *now){
    return time->getTimeDifferenceInMilliseconds(now, lastAccessTime);
}
