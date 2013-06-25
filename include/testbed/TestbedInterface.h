/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_INTERFACE_H_
#define _TESTBED_INTERFACE_H_

#include "Testbed.h"
#include "ReliableNetworkInterface.h"

TESTBED_NAMESPACE_BEGIN

class TestbedInterface : public ReliableNetworkInterface {
    public:
        void receive(Packet* packet);
        bool equals(NetworkInterface* otherInterface);
        bool operator==(ReliableNetworkInterface &otherInterface) {
            return this->equals(&otherInterface);
        }

    protected:
        void doSend(const Packet* packet, std::shared_ptr<Address> recipient);
};

TESTBED_NAMESPACE_END

#endif // _TESTBED_INTERFACE_H_
