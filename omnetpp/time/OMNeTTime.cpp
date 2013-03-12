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
        * Example:
        *    Transform from Seconds to
        *      millisec: 1 * 10^(0 + 3) = 1000
        *      microsec: 1 * 10^(0 + 6) = 1000000
        *    Transform from Milliseconds to
        *      millisec: 1 * 10^(-3 + 3) = 1
        *      microsec: 1 * 10^(-3 + 6) = 1000
        */
        return time.raw() * pow(10, time.getScaleExp() + wantedScaleExponent);
    }

}
