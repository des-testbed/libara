#include "OMNeTTime.h"

using namespace ARA;



int OMNeTTime::getTimeDifferenceInSeconds(SimTime *a, SimTime *b){
    return this->getTimeDifference(a, b, 0);
}

int OMNeTTime::getTimeDifferenceInMilliseconds(SimTime *a, SimTime *b){
     return this->getTimeDifference(a, b, -3);
}

int OMNeTTime::getTimeDifference(SimTime *a, SimTime *b, int precision){
    SimTime result = (*a - *b);
    // get the simulation time exponent
    if(result.getPrecision() != precision){
        return convertSimulationTime(result, precision)
    }
    return result.raw();
}

// check if there is a better way to do it
int converSimulationTime(SimTime r, int precision){
    /// fixme
    return 0;
}
