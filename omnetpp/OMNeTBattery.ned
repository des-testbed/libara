@namespace(ARA::omnetpp);
package ara;

import inet.battery.models.InetSimpleBattery;

simple OMNeTBattery {
    parameters:
        double capacity @unit("mAh");
        double voltage @unit("V");
        double updateInterval @unit("s");
}
