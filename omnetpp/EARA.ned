@namespace(ARA::omnetpp);
package ara;

simple EARA extends ARA {    
    parameters:
        @class(EARA);
        
        // Indicates the impact of the Minimum energy value when the energy fitness is initialized.
        // The value needs to be >= 1.value 1 <= b < 2 will favor the Minimum value over the Average.
     	// Everything > 2 will put more focus on the Average value. See the Master's Thesis of
     	// Friedrich Große for further explanation (corresponds to `b` in Energy initialization equation).
        double influenceOfMinimumEnergyValue;
        
        @statistic[dropPacketBecauseEnergyDepleted](title="Dropped packets because of depleted energy";record=count,vector);
}
