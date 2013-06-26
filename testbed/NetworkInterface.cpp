/*
 * $FU-Copyright$
 */

#include "testbed/NetworkInterface.h"

TESTBED_NAMESPACE_BEGIN

void NetworkInterface::receive(Packet* packet) {

}

bool NetworkInterface::equals(ARA::NetworkInterface* otherInterface) {
    return false;
}

void NetworkInterface::doSend(const Packet* packet, std::shared_ptr<Address> recipient){

}

TESTBED_NAMESPACE_END
