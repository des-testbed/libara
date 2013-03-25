@namespace(ARA::omnetpp);
package ara.evaporation;

simple OMNeTExponentialEvaporationPolicy
{
    parameters:
        double evaporationFactor = default(0.9);
        double threshold = default(1000.0);
}
