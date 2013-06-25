#include "TestbedInterface.h"

TESTBED_NAMESPACE_BEGIN

void receive(Packet* packet) {

}

bool equals(NetworkInterface* otherInterface) {
    return false;
}

void doSend(const Packet* packet, std::shared_ptr<Address> recipient){

}

TESTBED_NAMESPACE_END
