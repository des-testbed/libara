/*
 * $FU-Copyright$
 */

#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_

#include "Testbed.h"
#include "AbstractARAClient.h"
#include "ReliableNetworkInterface.h"

TESTBED_NAMESPACE_BEGIN

class TestbedNetworkInterface : public ReliableNetworkInterface {
    public:
        TestbedNetworkInterface(dessert_meshif_t* dessertPointer, AbstractARAClient* client, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds);
        ~TestbedNetworkInterface(){};

        bool equals(NetworkInterface* otherInterface);

    protected:
        void doSend(const Packet* packet, std::shared_ptr<Address> recipient);
};

TESTBED_NAMESPACE_END

#endif // _NETWORK_INTERFACE_H_
