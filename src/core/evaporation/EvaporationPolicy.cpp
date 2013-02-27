/*
 * $FU-Copyright$
 */

#include "EvaporationPolicy.h"

using namespace ARA;

EvaporationPolicy::EvaporationPolicy(TimeFactory* timeFactory) {
    this->interval = 100;
    this->factor = 1;
    this->timeFactory = timeFactory;
    this->lastAccessTime = nullptr;
}

EvaporationPolicy::~EvaporationPolicy(){
    delete this->timeFactory;

    if(tableHasBeenAccessedEarlier()) {
        delete lastAccessTime;
    }
} 

bool EvaporationPolicy::tableHasBeenAccessedEarlier() {
    return lastAccessTime != nullptr;
}

/**
 * The method checks if a given threshold is passed time-wise. If it is so, 
 * a factor is determined which denotes how often the evaporate method should
 * be called, e.g. assuming that the evaporation method should be called
 * every second and if 30 seconds have passed since the last access to the routing
 * table, the evaporation method has to be called 30 times. 
 *
 * @return The method returns true if the evaporation method should be triggered,
 *   false otherwise
 */
bool EvaporationPolicy::checkForEvaporation(){
    Time* currentTime = timeFactory->makeTime();
    currentTime->setToCurrentTime();

    if (tableHasBeenAccessedEarlier() == false) {
        lastAccessTime = currentTime;
        return false;
    }
    else {
        Time timeSinceLastAccess = *(currentTime) - *(this->lastAccessTime);
        long timeDifferenceInMilliSeconds = timeSinceLastAccess.toMilliseconds();
        delete currentTime;

        if(timeDifferenceInMilliSeconds >= interval) {
            this->determineEvaporationFactor(timeDifferenceInMilliSeconds);
            this->lastAccessTime->setToCurrentTime();
            return true;
        }
        else {
            return false;
        }
    }
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

