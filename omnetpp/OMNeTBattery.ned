@namespace(ARA::omnetpp);
package ara;

import inet.battery.models.InetSimpleBattery;

simple OMNeTBattery {
    parameters:
        double voltage @unit("V");
        double nominal @unit("mAh");
        double capacity @unit("mAh");
        double publishTime @unit("s");
}
