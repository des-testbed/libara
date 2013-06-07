@namespace(ARA::omnetpp);
package ara.evaporation;

import ara.evaporation.EvaporationPolicy;

simple OMNeTCubicEvaporationPolicy like EvaporationPolicy
{
    parameters:
        // The time which must have passed since the last traffic, until the
        // plateau center of the function is reached
        int timeUntilPlateauCenterIsReached @unit("ms");
        
        // The amount of pheromones which is evaporated at the plateau center
        // this is in respect to the pheromone value when the last traffic has been sent 
        double plateauCenterDepth;
        
        // The exponent if the evaporation function. This does not necessarily be 3
        // but it must be a positive odd number
        int exponent;
}
