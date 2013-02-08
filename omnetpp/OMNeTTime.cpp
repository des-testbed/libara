#include "OMNeTTime.h"

using namespace ARA;

OMNeTTime::OMNeTTime(){
  this->timestamp = SimTime();
}

OMNeTTime::OMNeTTime(SimTime timestamp):timestamp(timestamp){}

OMNeTTime::~OMNeTTime(){ }

OMNeTTime OMNeTTime::operator-(const OMNeTTime& right){
    SimTime result = (*this->getTimestamp() - *right.getTimestamp());
    return OMNeTTime(result);
}

OMNeTTime OMNeTTime::operator-=(const OMNeTTime& right){
    // fixme
    SimTime result = (*this->getTimestamp() - *right.getTimestamp());
    return OMNeTTime(result);
}

/**
 * The method returns a timestamp in seconds.
 * 
 * @return A timestamp in seconds
 */
int OMNeTTime::toSeconds(){
    return this->convertSimulationTime(0);
}

/**
 * The method returns a timestamp in milliseconds.
 * 
 * @return A timestamp in milliseconds
 */
long int OMNeTTime::toMilliseconds(){
    return this->convertSimulationTime(-3);
}

/**
 * TODO: change documentation of method
 * The method computes the time difference between two timestamps in OMNeT++ 
 * simulation time (as provided by the class SimTime). The method checks if
 * the desired time resolution meets the pre-set time resolution of the 
 * simulation (set in omnetpp.ini) which is by default picoseconds. If this
 * is not the case, the difference between the two timestamps is computed in
 * the desired time scale (e.g. seconds or milliseconds).
 *
 * @param a in The newer timestamp
 * @param b in The older timestamp
 * @param scaleExponent in The desired time scale
 *
 * @return The method returns the time difference between two timestamps based 
 *   on the desired time scale
 */
int OMNeTTime::convertSimulationTime(int scaleExponent){
    // get the simulation time exponent
    if(this->timestamp.getScaleExp() != scaleExponent){
       /**
        * TODO: Check if this is working the way it is intended to do
        *
        * Example:
        *    simulation time precision is already millisecond
        *    3000 / 10 ^ (-3 - -3) =  3000 / 10 ^ 0 = 3000
        *
        *    simulation time is picoseconds, but seconds desired
        *    3000 / 10 ^ (0 - -9) = 3000 / 10 ^ 9 = 0.000003 
        *
        * The method would return 0 for the latter example. The 
        * question if that is sufficient in an simulation scenario.
        */
        return (this->timestamp.raw() / pow(10, (scaleExponent - this->timestamp.getScaleExp())));
    }
    return this->timestamp.raw();
}


SimTime OMNeTTime::getTimestamp() const{
    return (this->timestamp);
}

bool OMNeTTime::isInitialized(){
    return (this->timestamp.raw() != 0);
}

void OMNeTTime::initialize(){
    /// TODO: check if that's the way to go
    this->timestamp = simTime();
}
