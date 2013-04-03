/*
 * $FU-Copyright$
 */

#ifndef TRAFFIC_GENERATOR_H_
#define TRAFFIC_GENERATOR_H_

#include "TrafGen.h"
#include "IPv4Address.h"
#include "IPv4Datagram.h"
#include "IPv4ControlInfo.h"

namespace ARA {
    namespace omnetpp {
	    /**
		 * The class provides a traffic generator for the ARA/EARA routing 
		 * protocols. It is based on the class TrafGen which is provided by
		 * the INETMANET framework in src/applications/trafgen/
		 */
        class TrafficGenerator : public TrafGen {
		    public:
			    virtual void initialize(int);
				virtual void finish();

			protected:
                virtual void handleSelfMsg(cPacket *);
                virtual void handleLowerMsg(cPacket *);
				
                virtual void SendTraf(cPacket *msg, const char *dest);

            private:
                int lowerLayerIn;
                int lowerLayerOut;

                int currentTrafficPattern;

                double numTrafficMsgs;
                double numTrafficMsgsRecv;

     //           ip_traffic_types ipTrafficType;
                double interDepartureTime, packetSize;
        };
    }
}

#endif
