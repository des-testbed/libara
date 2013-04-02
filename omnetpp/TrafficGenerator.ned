@namespace(ARA::omnetpp);
package ara;

import inet.applications.trafgen.TrafGen;

simple TrafficGenerator like TrafGen {
    parameters:
        @class(TrafficGenerator);
	    int defaultTrafConfigId = default(0);
        xml trafConfig;

    gates:
        inout lowergate;

}
