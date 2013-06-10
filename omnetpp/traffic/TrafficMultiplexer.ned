@namespace(ARA::omnetpp);
package ara.traffic;

simple TrafficMultiplexer
{
    parameters:
        int nrOfUpperGates;

        @display("i=block/join");
    gates:
        input fromUpperGate[nrOfUpperGates];
        output toLowerGate;

}
