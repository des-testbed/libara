@namespace(ARA::omnetpp);
package ara.evaporation;

import ara.evaporation.EvaporationPolicy;

simple OMNeTExponentialEvaporationPolicy like EvaporationPolicy
{
    parameters:
        double evaporationFactor = default(0.9);
        double threshold = default(1000.0);
}
