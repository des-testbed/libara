@namespace(ARA::omnetpp);
package ara;

simple ARA {
    
    parameters:
        double initialPhi = default(0);
        int nrOfRouteDiscoveryRetries = default(2);
        int routeDiscoveryTimeout @unit("ms") = default (1000s);
        int maxTTL = default(20); // the maximum number of hops all packets are allowed to travel
        int nrOfNetworkInterfaces = default(1);
        volatile double broadCastDelay @unit("s") = default (uniform(0s,0.005s));  // the delay added to broadcast operations to prevent packet collision by perfect synchronization in the simulation 
        volatile double uniCastDelay @unit("s") = default (0s);  // a delay added to unicast messaged (i.e. data packet forwarding) (used to model processing time)

    gates:
        inout  upperLayerGate;
        input  ifIn[nrOfNetworkInterfaces];		// input from NIC interface (INET/MANET needs this to be named this way) 
        output ifOut[nrOfNetworkInterfaces];	// output to  NIC interface (INET/MANET needs this to be named this way)
        input arpIn;                            // input from ARP
        output arpOut;                          // output to ARP
}
