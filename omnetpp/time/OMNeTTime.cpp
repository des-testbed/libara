/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTTime.h"

#include <cmath>

using namespace ARA;
using namespace ARA::omnetpp;


void OMNeTTime::setToCurrentTime(){
    timestamp = simTime();
}

long OMNeTTime::getDifferenceInMilliSeconds(const Time* otherTime) const {
    const OMNeTTime* omnetTime = dynamic_cast<const OMNeTTime*>(otherTime);
    if(omnetTime) {
        SimTime result = this->timestamp - omnetTime->timestamp;
        return convertSimulationTime(result, SimTime::SCALEEXP_MS);
    }
    else {
        throw cRuntimeError("Can only subtract other OMNeTTime instances from this OMNeTTime instance");
    }
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
long OMNeTTime::convertSimulationTime(SimTime time, int wantedScaleExponent) const {
    if (time.getScaleExp() == wantedScaleExponent) {
        return time.raw();
    }
    else {
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
        return time.raw() / pow(10, (wantedScaleExponent - time.getScaleExp()));
    }

}
