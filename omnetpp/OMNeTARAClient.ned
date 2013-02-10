@namespace(ARA::omnetpp);
package ara;

simple OMNeTARAClient
{
    parameters:
        string policy;
        double deltaPhi;
        double initialPhi;

    gates:
        inout g[];
}
