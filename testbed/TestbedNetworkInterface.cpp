/*
 * $FU-Copyright$
 */

#include "TestbedNetworkInterface.h"
#include "TestbedAddress.h"
#include "TestbedPacketDispatcher.h"

TESTBED_NAMESPACE_BEGIN

AddressPtr TestbedNetworkInterface::broadcastAddress = std::dynamic_pointer_cast<Address>(std::make_shared<TestbedAddress>(DESSERT_BROADCAST_ADDRESS));
extern NetworkInterfaceMap networkInterfaces;

TestbedNetworkInterface::TestbedNetworkInterface(dessert_meshif_t* dessertPointer, AbstractARAClient* client, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds) 
  : ReliableNetworkInterface(client, ackTimeoutInMicroSeconds, std::dynamic_pointer_cast<Address>(std::make_shared<TestbedAddress>(dessertPointer->hwaddr)), broadcastAddress) {
    this->dessertPointer = dessertPointer;
    registerInterface();
}

TestbedNetworkInterface::~TestbedNetworkInterface() {
    networkInterfaces.erase(this->dessertPointer);
}

bool TestbedNetworkInterface::isRegistered() {
    return extractNetworkInterface(dessertPointer) == this;
}

bool TestbedNetworkInterface::equals(NetworkInterface* otherInterface) {
    TestbedNetworkInterface* other = (TestbedNetworkInterface*)otherInterface;
    return (this->dessertPointer->hwaddr == other->dessertPointer->hwaddr);
}

dessert_meshif_t* TestbedNetworkInterface::getDessertPointer()  const {
    return this->dessertPointer;
}

void TestbedNetworkInterface::doSend(const Packet* packet, std::shared_ptr<Address> recipient){
    // TODO
    printf(" [TestbedNetworkInterface] sending packet # %u type %s over interface at %s \n", packet->getSequenceNumber(), PacketType::getAsString(packet->getType()).c_str(), this->getLocalAddress()->toString().c_str());
    packetToMeshInterfaceDispatcher(packet, this, recipient);
}

void TestbedNetworkInterface::registerInterface() {
    networkInterfaces[this->getDessertPointer()] = this;
}

TESTBED_NAMESPACE_END
