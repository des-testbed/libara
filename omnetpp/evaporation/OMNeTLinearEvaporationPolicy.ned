@namespace(ARA::omnetpp);
package ara.evaporation;

simple OMNeTLinearEvaporationPolicy
{
    parameters:
        double evaporationFactor = default(0.9);
        double threshold = default(1000.0);
}
