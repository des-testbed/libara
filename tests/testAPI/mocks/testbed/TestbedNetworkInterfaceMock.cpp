/*
 * $FU-Copyright$
 */

#include "TestbedNetworkInterfaceMock.h"

using namespace std;

TESTBED_NAMESPACE_BEGIN
TestbedNetworkInterfaceMock::TestbedNetworkInterfaceMock(dessert_meshif_t* dessertPointer, AbstractNetworkClient* client, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds) :
    TestbedNetworkInterface(dessertPointer, client, packetFactory, ackTimeoutInMicroSeconds) {

}
/*
string TestbedNetworkInterfaceMock::DEFAULT_INTERFACE_NAME = "NetworkInterfaceMock";
AddressPtr TestbedNetworkInterfaceMock::DEFAULT_LOCAL_ADDRESS = std::make_shared<AddressMock>("DEFAULT");
AddressPtr TestbedNetworkInterfaceMock::DEFAULT_BROADCAST_ADDRESS = std::make_shared<AddressMock>("BROADCAST");
int TestbedNetworkInterfaceMock::DEFAULT_ACK_TIMEOUT = 5000;
*/
TESTBED_NAMESPACE_END
