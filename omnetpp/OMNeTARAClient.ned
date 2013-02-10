@namespace(ARA::omnetpp);
package ara;

simple OMNeTARAClient
{
    parameters:
        string forwardingPolicy = default("stochastic");
        double deltaPhi = default(1);
        double initialPhi;
        int nrOfNetworkInterfaces = default(1);

    gates:
        inout  upperLayerGate;
        input  ifIn[nrOfNetworkInterfaces];		// input from NIC interface (INET/MANET needs this to be named this way) 
        output ifOut[nrOfNetworkInterfaces];	// output to  NIC interface (INET/MANET needs this to be named this way)
        input arpIn;                            // input from ARP
        output arpOut;                          // output to ARP
}
