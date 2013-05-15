@namespace(ARA::omnetpp);
package ara.evaporation;

import ara.evaporation.EvaporationPolicy;

simple OMNeTCubicEvaporationPolicy like EvaporationPolicy
{
    parameters:
        double plateau;
        double slow;
        double reduction;
        double threshold;
}
