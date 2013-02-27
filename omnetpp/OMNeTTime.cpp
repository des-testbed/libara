/*
 * $FU-Copyright$
 */

#include "OMNeTTime.h"

using namespace ARA;

OMNeTTime::OMNeTTime(){
  this->timestamp = SimTime();
}

OMNeTTime::OMNeTTime(SimTime timestamp):timestamp(timestamp){}

OMNeTTime::~OMNeTTime(){ }

/**
 * The method provides the subtraction of two timestamps using
 * the '-' operator.
 * 
 * @return A new instance of a OMNeT++ simulation time timestamp (encapsulated in class OMNeTTime)
 */
OMNeTTime OMNeTTime::operator-(const OMNeTTime& right){
    SimTime result = (this->getTimestamp() - right.getTimestamp());
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
 * The method converts the simulation time to the desired time scale as specified in
 * the parameter of method. 
 *
 * @param scaleExponent in The desired time scale
 *
 * @return The method returns the (raw) simulation time in the
 *   desired time scale
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

/**
 * The method returns the simulation time member variable
 *
 * @return The simulation time member variable
 */
SimTime OMNeTTime::getTimestamp() const{
    return (this->timestamp);
}

void OMNeTTime::setToCurrentTime(){
    /// TODO: check if that's the way to go
    this->timestamp = simTime();
}
