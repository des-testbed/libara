#include "OMNeTTime.h"

using namespace ARA;

/**
 * The method returns the time difference between two OMNeT++SimTime instances in 
 * seconds.
 *
 * @param a in The newer timestamp
 * @param b in The older timestamp
 * 
 * @return The time difference in seconds between two timestamps
 */
int OMNeTTime::getTimeDifferenceInSeconds(SimTime *a, SimTime *b){
    return this->getTimeDifference(a, b, 0);
}

/**
 * The method returns the time difference between two OMNeT++SimTime instances in 
 * milliseconds.
 *
 * @param a in The newer timestamp
 * @param b in The older timestamp
 * 
 * @return The time difference in milliseconds between two timestamps
 */
int OMNeTTime::getTimeDifferenceInMilliseconds(SimTime *a, SimTime *b){
     return this->getTimeDifference(a, b, -3);
}

/**
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
int OMNeTTime::getTimeDifference(SimTime *a, SimTime *b, int scaleExponent){
    SimTime result = (*a - *b);
    // get the simulation time exponent
    if(result.getScaleExp() != scaleExponent){
        return convertSimulationTime(result, scaleExponent);
    }
    return result.raw();
}

// check if there is a better way to do it
int OMNeTTime::convertSimulationTime(SimTime r, int scaleExponent){
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
    return (r.raw() / pow(10, (scaleExponent - r.getScaleExp())));
}
