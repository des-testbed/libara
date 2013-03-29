@namespace(ARA::omnetpp);
package ara;

import inet.battery.models.InetSimpleBattery;

simple OMNeTBattery extends InetSimpleBattery {
    parameters:
	    @class(OMNeTBattery);
        ConsumedVector = true;
}
