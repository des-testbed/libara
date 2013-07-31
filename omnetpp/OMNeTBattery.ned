@namespace(ARA::omnetpp);
package ara;

import inet.battery.models.InetSimpleBattery;

simple OMNeTBattery {
    parameters:
        double capacity @unit("mAh");
        double voltage @unit("V");  
        double updateInterval @unit("s");
        
        // the fraction the energy has to change (compared to the capacity)
        // before it is written via the out vector
        // must be between 0 and 1 (inclusive)
        double energyVectorDelta = default(0.01);  
}
