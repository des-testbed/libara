#include "EvaporationPolicy.h"

#include <iostream>

using namespace ARA;

EvaporationPolicy::EvaporationPolicy():factor(1),interval(100){ 
    this->lastAccessTime = new Time();
}

EvaporationPolicy::EvaporationPolicy(Time *time):lastAccessTime(time),factor(1),interval(100){}

EvaporationPolicy::~EvaporationPolicy(){
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
    if(this->lastAccessTime->isInitialized()){
        /// get the current date
        Time* now = new Time();
        /// compute the time difference
        long int timeDifference = (*(this->lastAccessTime) - *(now)).toMilliseconds();
        /// compare the timestamps 
        if(timeDifference > this->interval){
            /// compute the factor
            determineEvaporationFactor(timeDifference);
            /// delete the last access time
            delete this->lastAccessTime;
            /// update the timestamp
            this->lastAccessTime = now;
            /// return the result
            return true;
        }
    }else{
       this->lastAccessTime->initialize();
    }

    return false;
}

/** 
 *
 */
void EvaporationPolicy::determineEvaporationFactor(int timeDifference){
    this->factor = (uint8_t)(timeDifference/this->interval);
}

int EvaporationPolicy::getFactor(){
    return this->factor;
}

void EvaporationPolicy::setInterval(int interval){
    this->interval = interval;
}

