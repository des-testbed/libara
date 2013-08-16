@namespace(ARA::omnetpp);
package ara.forwarding;

import ara.forwarding.ForwardingPolicy;

simple OMNeTEnergyAwareStochasticForwardingPolicy like ForwardingPolicy
{
    parameters:
        double alpha;
        double beta;
}
