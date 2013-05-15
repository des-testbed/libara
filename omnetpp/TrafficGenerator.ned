@namespace(ARA::omnetpp);
package ara;

import inet.applications.trafgen.TrafGen;

simple TrafficGenerator like TrafGen {
    parameters:
        @class(TrafficGenerator);
	    int defaultTrafConfigId = default(0);
        xml trafConfig;
        int nrOfPackets = default(-1); // number of packets to be send by this traffic generator. -1 disables this constraint

    gates:
        inout lowergate;

}
