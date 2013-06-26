/*
 * $FU-Copyright$
 */

#include "testbed/NetworkInterface.h"
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

AddressPtr NetworkInterface::localAddress = AddressPtr(new TestbedAddress(DESSERT_LOCAL_ADDRESS));
AddressPtr NetworkInterface::broadcastAddress = AddressPtr(new TestbedAddress(DESSERT_BROADCAST_ADDRESS));

NetworkInterface::NetworkInterface(AbstractARAClient* client, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds)
                        : ReliableNetworkInterface(client, packetFactory, ackTimeoutInMicroSeconds, localAddress, broadcastAddress) {

}

void NetworkInterface::receive(Packet* packet) {
    //TODO
}

bool NetworkInterface::equals(ARA::NetworkInterface* otherInterface) {
    //TODO
    return false;
}

void NetworkInterface::doSend(const Packet* packet, std::shared_ptr<Address> recipient){
    //TODO
}

TESTBED_NAMESPACE_END
