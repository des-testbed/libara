@namespace(ARA::omnetpp);
package ara;

simple EARA extends ARA {    
    parameters:
        @class(EARA);
        int helloMessageTimeout @unit(ms) = default(1000ms);
        
        @statistic[dropPacketBecauseEnergyDepleted](title="Dropped packets because of depleted energy";record=count,vector);
}
