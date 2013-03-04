/*
 * $FU-Copyright$
 */

#include "OMNeTTime.h"

using namespace ARA;
using namespace ARA::omnetpp;

OMNeTTime::OMNeTTime() {
    //FIXME setToCurrentTime(); // does not work in the tests
}

OMNeTTime::OMNeTTime(SimTime timestamp) {
    this->timestamp = timestamp;
}

long OMNeTTime::getDifferenceInMilliSeconds(const Time* right) const {
    /*FIXME const OMNeTTime* omnetTime = dynamic_cast<const OMNeTTime*>(right);
    if(omnetTime) {
        SimTime result = (this->getTimestamp() - omnetTime->getTimestamp());
        return new OMNeTTime(result);
    }
    else {
        throw cRuntimeError("Can only subtract other OMNeTTime instances from this OMNeTTime instance");
    }*/
    return 0;
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
int OMNeTTime::convertSimulationTime(int scaleExponent) const {
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

void OMNeTTime::setToCurrentTime() {
    //timestamp = simTime(); // this throws a segfault in the test
}
