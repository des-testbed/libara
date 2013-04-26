@namespace(ARA::omnetpp);
package ara.evaporation;

import ara.evaporation.EvaporationPolicy;

// This class provides the exponential evaporation function of the ant routing algorithm (ARA)
// The pheromones will be evaporated every x milliseconds (time interval)
// according to the following formula: newPhi = oldPhi * factor
simple OMNeTExponentialEvaporationPolicy like EvaporationPolicy {
    parameters:
        double evaporationFactor; // The factor which is used to evaporate the pheromone values
        int timeInterval @unit(ms) = default(1000ms); // denotes the time interval in milliseconds at which the full evaporation factor should be applied
        int minTimeDifferenceBeforeEvaporation @unit(ms) = default(30ms); // the minimum time that must have been passed before the evaporation function is partially applied
        double threshold = default(0); // denotes the smallest allowed pheromone value before it is set to 0
}
