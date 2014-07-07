/*
 * $FU-Copyright$
 */

#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_

#include "Testbed.h"
#include "AbstractNetworkClient.h"
#include "ReliableNetworkInterface.h"

#include <mutex>

TESTBED_NAMESPACE_BEGIN

class TestbedNetworkInterface : public ReliableNetworkInterface {
    public:
        TestbedNetworkInterface(dessert_meshif_t* dessertPointer, AbstractNetworkClient* client, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds);
        ~TestbedNetworkInterface(){};

        void send(const Packet* packet, AddressPtr recipient);
        void receive(Packet* packet);
        bool equals(NetworkInterface* otherInterface);

    protected:
        void doSend(const Packet* packet, std::shared_ptr<Address> recipient);

    private:
        std::mutex unacknowledgedPacketsMutex;
        std::mutex acknowledgmentTimerMutex;
        std::mutex receiveMutex;
        std::mutex sendMutex;
};

TESTBED_NAMESPACE_END

#endif // _NETWORK_INTERFACE_H_
