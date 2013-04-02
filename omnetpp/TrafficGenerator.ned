@namespace(ARA::omnetpp);
package ara;

simple TrafficGenerator {
    parameters:
	    int defaultTrafConfigId;
        xml trafConfig;

    gates:
        input lowerLayerIn;
        output lowerLayerOut;
}
