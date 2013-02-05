@namespace(ARA);
package ara_simulation.simulations;

simple OMNeTARAClient
{
    parameters:
        string policy;
        double deltaPhi;
        double initialPhi;

    gates:
        inout g[];
}
