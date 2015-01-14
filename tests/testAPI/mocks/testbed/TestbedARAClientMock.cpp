/*
 * $FU-Copyright$
 */

#include "TestbedARAClientMock.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClientMock::TestbedARAClientMock() {
    BasicConfiguration configuration = getStandardConfiguration();
    initialize(configuration);
}

TestbedARAClientMock::TestbedARAClientMock(Configuration& configuration) {
    initialize(configuration);
}

BasicConfiguration TestbedARAClientMock::getStandardConfiguration() const {
    float initialPhi = 5.0;
    float deltaPhi = 5.0;
    TestbedRoutingTable* routingTable = new TestbedRoutingTableMock();
    std::shared_ptr<TestbedPacketTrap> packetTrap = std::make_shared<TestbedPacketTrap>(routingTable);

    return BasicConfiguration(
        routingTable,
        new TestbedPacketFactory(15),
        new ExponentialEvaporationPolicyMock(),
        new LinearPathReinforcementPolicy(deltaPhi),
        new BestPheromoneForwardingPolicy(routingTable),
        packetTrap,
        initialPhi
    );
}

std::shared_ptr<TestbedPacketTrap> TestbedARAClientMock::getPacketTrap(){
    return std::dynamic_pointer_cast<TestbedPacketTrap>(packetTrap);
}
/*
TestbedPacketFactory* TestbedARAClientMock::getPacketFactory() const {
    return dynamic_cast<TestbedPacketFactory*>(packetFactory);
}*/

NetworkInterfaceMock* TestbedARAClientMock::createNewNetworkInterfaceMock(const std::string localAddressName) {
    NetworkInterfaceMock* mock = AbstractClientMockBase::createNewNetworkInterfaceMock(localAddressName);
    addNetworkInterface(mock);
    return mock;
}

TestbedNetworkInterfaceMock* TestbedARAClientMock::createNewTestbedNetworkInterfaceMock(const std::string localAddressName) {
    std::stringstream mockName;
    mockName << "InterfaceMock" << (interfaceMocks.size() + 1);

    std::string interface = mockName.str();

    std::shared_ptr<TestbedAddress> local = std::make_shared<TestbedAddress>(DESSERT_BROADCAST_ADDRESS);
    std::shared_ptr<TestbedAddress> broadcast = std::make_shared<TestbedAddress>(DESSERT_BROADCAST_ADDRESS);

    TestbedNetworkInterfaceMock* mock = new TestbedNetworkInterfaceMock(interface, this, local, broadcast, this->getPacketFactory(), 600);
    addNetworkInterface(mock);
    return mock;
}

//FIXME
TestbedRoutingTable* TestbedARAClientMock::getRoutingTable() {
    return dynamic_cast<TestbedRoutingTable*>(routingTable);
}

TESTBED_NAMESPACE_END
