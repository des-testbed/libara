/*
 * $FU-Copyright$
 */

#include "testbed/NetworkInterface.h"
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

AddressPtr NetworkInterface::localAddress = AddressPtr(new TestbedAddress(DESSERT_LOCAL_ADDRESS));
AddressPtr NetworkInterface::broadcastAddress = AddressPtr(new TestbedAddress(DESSERT_BROADCAST_ADDRESS));

NetworkInterface::NetworkInterface(std::string name, AbstractARAClient* client, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds)
                        : ReliableNetworkInterface(client, packetFactory, ackTimeoutInMicroSeconds, localAddress, broadcastAddress) {
    this->name = name;
}

void NetworkInterface::receive(Packet* packet) {
        deliverToARAClient(packet);
}

void NetworkInterface::deliverToARAClient(Packet* packet) {
        client->receivePacket(packet, this);
}

bool NetworkInterface::equals(ARA::NetworkInterface* otherInterface) {
    ARA::testbed::NetworkInterface* other = (ARA::testbed::NetworkInterface*)otherInterface;
    return this->name == other->name;
}

void NetworkInterface::doSend(const Packet* packet, std::shared_ptr<Address> recipient){
    //TODO
}

TESTBED_NAMESPACE_END
