@namespace(ARA::omnetpp);
package ara;

simple ARA {
    
    parameters:
        // The value which is added to the initial pheromone value which is based on the TTL.
        // This can be used to hold the TTL as low as possible without making the initial pheromone value always too small
        double initialPhi = default(0);
        
        // The number of route discovery retries.
        // This controls how often a client starts a new route discovery if the current discovery has failed.
        // If this number is exceeded the client drops all trapped packets for that destination and does no longer try to find a route. 
        int nrOfRouteDiscoveryRetries = default(3);
        
        // TODO write documentation
        int routeDiscoveryTimeout @unit("ms");
        
        // TODO enhance documentation
        int packetDeliveryDelay @unit("ms") = default(8ms); // the time a client waits after the first received BANT for additional BANTs coming back from the destination before it starts to send DATA packets
        
         // The maximum number of hops all packets are allowed to travel.
         // This does influences the initial pheromone value.
        int maxTTL;
        
        // If this value is greater 0 the client will check each neighborActivityCheckInterval milliseconds
        // if there are neighbors which have been inactive for to long. If so they are contacted which could
        // lead to a deletion from the routing table if they are no longer reachable. This is mostly important in
        // mobile scenarios where the connection graph may change heavily. 
        int neighborActivityCheckInterval @unit("ms") = default(0ms);
        
        // If the clients check for the last time a neighbor has been seen active
        // it will try to contact each client which has not been active for more than this value in milliseconds.
        int maxNeighborInactivityTime @unit("ms") = default(1000ms);
        
        // The PANTInterval controls if and when PANTs are sent into the network to explore new routes
        // The default value of 0 means that this feature is disabled
        int pantInterval @unit("ms") = default(0ms);
        
        string logLevel @enum("TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL") = default("INFO");
        
        int nrOfNetworkInterfaces = default(1);
        volatile double broadCastDelay @unit("s") = default(uniform(0s,0.003s));  // the delay added to broadcast operations to prevent packet collision by perfect synchronization in the simulation 
        volatile double uniCastDelay @unit("s") = default(0s);  // a delay added to unicast messaged (i.e. data packet forwarding) (used to model processing time)
        
        int routingTableStatisticsUpdate @unit("ms") = default(500ms)
        @statistic[packetDelivered](title="Packets devlivered";record=count,vector);
        @statistic[packetUnDeliverable](title="Packets undevliverable";record=count,vector);
        @statistic[routingLoopDetected](title="Routing loops";record=count,vector);
        @statistic[routeFailure](title="Route Failures";record=count,vector);
        @statistic[dropZeroTTLPacket](title="Zero TTL drops";record=count,vector);
        @statistic[routeFailureNoHopAvailable](title="Route Failure (no hop available)";record=count);
        @statistic[routeFailureNextHopIsSender](title="Route Failure (next hop = sender)";record=count);
        @statistic[newRouteDiscovery](title="Nr of Route Discoveries";record=count);

    gates:
        inout  upperLayerGate;
        input  ifIn[nrOfNetworkInterfaces];		// input from NIC interface (INET/MANET needs this to be named this way) 
        output ifOut[nrOfNetworkInterfaces];	// output to  NIC interface (INET/MANET needs this to be named this way)
}
