@namespace(ARA::omnetpp);
package ara;

simple ARA {
    
    parameters:
        double initialPhi = default(0);
        int nrOfRouteDiscoveryRetries = default(3);
        int routeDiscoveryTimeout @unit("ms");
        int packetDeliveryDelay @unit("ms") = default(8ms); // the time a client waits after the first received BANT for additional BANTs coming back from the destination before it starts to send DATA packets
        int maxTTL; // the maximum number of hops all packets are allowed to travel. This does also influence the initial phi value
        int nrOfNetworkInterfaces = default(1);
        volatile double broadCastDelay @unit("s") = default(uniform(0s,0.003s));  // the delay added to broadcast operations to prevent packet collision by perfect synchronization in the simulation 
        volatile double uniCastDelay @unit("s") = default(0s);  // a delay added to unicast messaged (i.e. data packet forwarding) (used to model processing time)
        
        int routingTableStatisticsUpdate @unit("ms") = default(500ms)
        @statistic[packetDelivered](title="Packets devlivered";record=count,vector);
        @statistic[packetUnDeliverable](title="Packets undevliverable";record=count,vector);
        @statistic[routingLoopDetected](title="Routing loops";record=count,vector);
        @statistic[routeFailure](title="Route Failures";record=count,vector);
        @statistic[dropZeroTTLPacket](title="Zero TTL drops";record=count,vector);
        @statistic[nonSourceRouteDiscovery](title="Non-source Route Discovery";record=count);
        @statistic[newRouteDiscovery](title="Nr of Route Discoveries";record=count);

    gates:
        inout  upperLayerGate;
        input  ifIn[nrOfNetworkInterfaces];		// input from NIC interface (INET/MANET needs this to be named this way) 
        output ifOut[nrOfNetworkInterfaces];	// output to  NIC interface (INET/MANET needs this to be named this way)
}
