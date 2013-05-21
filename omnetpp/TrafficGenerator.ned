@namespace(ARA::omnetpp);
package ara;

import inet.applications.trafgen.TrafGen;

simple TrafficGenerator like TrafGen {
    parameters:
        @class(TrafficGenerator);
        xml trafConfig = default(xmldoc("traffic.xml"));
	    int defaultTrafConfigId = default(-1); // disabled per default
        int nrOfPackets = default(-1); // number of packets to be send by this traffic generator. -1 disables this constraint

    gates:
        inout lowergate;

}
