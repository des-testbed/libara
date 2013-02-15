#include "EvaporationPolicy.h"

#include <iostream>
#include <typeinfo> 

using namespace ARA;

EvaporationPolicy::EvaporationPolicy():factor(1),interval(100){ 
    this->lastAccessTime = new Time();
    this->now = new Time();
}

EvaporationPolicy::EvaporationPolicy(Time *last, Time *now):factor(1),lastAccessTime(last),now(now),interval(100){}

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
        Time bla = *(this->now) - *(this->lastAccessTime);
        //long int timeDifference = (*(this->now) - *(this->lastAccessTime)).toMilliseconds();
        long int timeDifference = 0;
        // DEBUG:       std::cout << "# " << this->now->toSeconds() << " " << this->lastAccessTime->toMilliseconds() << std::endl;
        // DEBUG:       std::cout << "time difference " << timeDifference << std::endl;

        /// compare the timestamps 
        if(timeDifference >= this->interval){
            /// compute the factor
            determineEvaporationFactor(timeDifference);
            /// update the timestamp
            this->lastAccessTime->update(*(this->now));
            /// return the result
            return true;
        }
    }else{
       // check
       this->now->initialize();
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

