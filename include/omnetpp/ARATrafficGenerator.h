/*
 * $FU-Copyright$
 */

#ifndef __ARA_SIMULATION_ARATRAFFICGENERATOR_H_
#define __ARA_SIMULATION_ARATRAFFICGENERATOR_H_

#include "IPvXAddress.h"

#include <omnetpp.h>
#include <vector>

namespace ARA {
namespace omnetpp {

    class ARATrafficGenerator : public cSimpleModule {
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

    private:
        short protocolType;
        unsigned int messageByteLength;
        unsigned int numberOfPackets;
        unsigned int numberOfSentPackets = 0;
        unsigned int defaultBroadCastAddressInterface;

        std::vector<IPvXAddress> destinationAddresses;

        void readDestinationAddresses(const char* destinationAddressesParameter);
        void sendPacket();
        IPvXAddress chooseDestinationAddress();
        void printPacket(cPacket* msg);
    };
}
}

#endif
