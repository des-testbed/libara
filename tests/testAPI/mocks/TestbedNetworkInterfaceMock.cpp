/*
 * $FU-Copyright$
 */


#include "TestbedNetworkInterfaceMock.h"

#include <string>

TESTBED_NAMESPACE_BEGIN

std::string TestbedNetworkInterfaceMock::DEFAULT_INTERFACE_NAME = "TestbedNetworkInterfaceMock";
int TestbedNetworkInterfaceMock::DEFAULT_ACK_TIMEOUT = 5000;

TestbedNetworkInterfaceMock::TestbedNetworkInterfaceMock(dessert_meshif_t* interface, AbstractNetworkClient* client) : TestbedNetworkInterface(interface, client, client->getPacketFactory(), DEFAULT_ACK_TIMEOUT) {
    this->name = DEFAULT_INTERFACE_NAME;
}

TestbedNetworkInterfaceMock::TestbedNetworkInterfaceMock(dessert_meshif_t* interface, const std::string interfaceName, AbstractNetworkClient* client) : TestbedNetworkInterface(interface, client, client->getPacketFactory(), DEFAULT_ACK_TIMEOUT)  {
    this->name = interfaceName;
}

TESTBED_NAMESPACE_END

