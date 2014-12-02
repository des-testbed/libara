/*
 * $FU-Copyright$
 */

#include "TestbedARAClientMock.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClientMock::TestbedARAClientMock() {

}

BasicConfiguration TestbedARAClientMock::getStandardConfiguration() const {
    float initialPhi = 5.0;
    float deltaPhi = 5.0;
    TestbedRoutingTable* routingTable = new RoutingTableMock();
    std::shared_ptr<TestbedPacketTrap> packetTrap = std::make_shared<TestbedPacketTrap>(routingTable);

    return BasicConfiguration(
        routingTable,
        new TestbedPacketFactory(15),
        new ExponentialEvaporationPolicyMock(),
        new LinearPathReinforcementPolicy(deltaPhi),
        new BestPheromoneForwardingPolicy(routingTable),
        packetTrap,
        initialPhi
}

TESTBED_NAMESPACE_END
