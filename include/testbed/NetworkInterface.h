/*
 * $FU-Copyright$
 */

#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_

#include "Testbed.h"
#include "ReliableNetworkInterface.h"

TESTBED_NAMESPACE_BEGIN

class NetworkInterface : public ReliableNetworkInterface {
    public:
        NetworkInterface(std::string name, AbstractARAClient* client, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds);
        void receive(Packet* packet);
        bool equals(ARA::NetworkInterface* otherInterface);

    protected:
        void doSend(const Packet* packet, std::shared_ptr<Address> recipient);

        static AddressPtr localAddress;
        static AddressPtr broadcastAddress;
        static std::string name;
};

TESTBED_NAMESPACE_END

#endif // _TESTBED_INTERFACE_H_
