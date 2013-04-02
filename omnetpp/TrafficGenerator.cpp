#include "omnetpp/TrafficGenerator.h"

namespace ARA {
    namespace omnetpp {
        Define_Module(TrafficGenerator);

        void TrafficGenerator::initialize(int stage) {
		    TrafGen::initialize(stage);

			if (stage == 0) {
                lowerLayerIn = findGate("lowerLayerIn");
                lowerLayerOut = findGate("lowerLayerOut");

                numTrafficMsgs = 0;

            }
        }

        void TrafficGenerator::handleLowerMsg(cPacket *message) {
            delete message;
		}

        void TrafficGenerator::finish() {
            recordScalar("trafficSent", numTrafficMsgs);
//            recordScalar("Bitrate of generated traffic", (1 / interDepartureTime * packetSize) + 0.5);
        }

		void TrafficGenerator::handleSelfMsg(cPacket *message){
            TrafGen::handleSelfMsg(message);
		}

		void TrafficGenerator::SendTraf(cPacket *message, const char *destination) {
            IPv4Datagram *datagram = new IPv4Datagram("ip_datagram");
            datagram->encapsulate(message);

            IPv4ControlInfo* controlInfo = new  IPv4ControlInfo();
            IPv4Address sourceAddress("192.168.0.1");
            IPv4Address destinationAddress(destination);
            /// set source
            controlInfo->setSrcAddr(sourceAddress);
            /// set destination
            controlInfo->setDestAddr(destinationAddress);
			/// increment the message counter
            numTrafficMsgs++;
            /// send the IPv4 datagram to the lower layer 

            datagram->setControlInfo(controlInfo);

            send(datagram, lowerLayerOut); 
        }
    }
}
