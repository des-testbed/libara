@namespace(ARA::omnetpp);
package ara.evaporation;

import ara.evaporation.EvaporationPolicy;

simple OMNeTExponentialEvaporationPolicy like EvaporationPolicy
{
    parameters:
        double evaporationFactor;
        double threshold;
}
