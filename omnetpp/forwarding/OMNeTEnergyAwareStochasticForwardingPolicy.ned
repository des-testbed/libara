@namespace(ARA::omnetpp);
package ara.forwarding;

import ara.forwarding.ForwardingPolicy;

simple OMNeTEnergyAwareStochasticForwardingPolicy like ForwardingPolicy
{
    parameters:
        double alpha = default(1.0);
        double beta = default(2.0);
}
