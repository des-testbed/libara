#include "EvaporationPolicy.h"

#include <iostream>
#include <typeinfo> 

using namespace ARA;

EvaporationPolicy::EvaporationPolicy(Time *last, Time *now) {
    this->factor = 1;
    this->lastAccessTime = last;
    this->now = now;
    this->interval = 100;
}

EvaporationPolicy::~EvaporationPolicy(){
    delete this->now;
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
        now->update();

        long int timeDifference = (*(this->now) - *(this->lastAccessTime)).toMilliseconds();

        /// compare the timestamps 
        if(timeDifference >= this->interval){
            /// compute the factor
            this->determineEvaporationFactor(timeDifference);
            /// update the timestamp
            this->lastAccessTime->update(*(this->now));

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

