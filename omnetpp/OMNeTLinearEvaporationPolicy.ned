@namespace(ARA::omnetpp);
package ara;

simple OMNeTLinearEvaporationPolicy
{
    parameters:
        double evaporationFactor = default(0.9);
        double threshold = default(1000.0);
}
