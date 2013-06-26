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
        void receive(Packet* packet);
        bool equals(ARA::NetworkInterface* otherInterface);

    protected:
        void doSend(const Packet* packet, std::shared_ptr<Address> recipient);
};

TESTBED_NAMESPACE_END

#endif // _TESTBED_INTERFACE_H_
