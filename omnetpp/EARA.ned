@namespace(ARA::omnetpp);
package ara;

simple EARA extends ARA {    
    parameters:
        @class(EARA);
        
        // Indicates the maximum energy capacity which is expected in the network.
     	// This will be used to calculate the percentage values from the received energy.
        int maximumEnergyValue = default(255);
        
        // Indicates the impact of the Minimum energy value when the energy fitness is initialized.
        // The value needs to be >= 1.value 1 <= b < 2 will favor the Minimum value over the Average.
     	// Everything > 2 will put more focus on the Average value. See the Master's Thesis of
     	// Friedrich Große for further explanation (corresponds to `b` in Energy initialization equation).
        double influenceOfMinimumEnergyValue;
        
        // The time a client waits for other arriving ant packets in the route discovery phase
        // before it rebroadcasts the packet to its neighbors. This gives routes with a higher delay
        // but better energy fitness a chance of beeing part of the route discovery
        // See the Master's Thesis of Große for further explanation. 
        int routeDiscoveryDelay @unit(mS) = default(2mS);
        
        // Returns the percent a destination clients energy has to be reduced, before it broadcasts a PEANT.
     	// A value of -1 means this feature is disabled.
        double peantEnergyThreshold = default(0.01);
        
        @statistic[dropPacketBecauseEnergyDepleted](title="Dropped packets because of depleted energy";record=count,vector);
}
