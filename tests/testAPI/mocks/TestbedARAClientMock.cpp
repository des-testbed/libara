/*
 * $FU-Copyright$
 */
#include "TestbedARAClientMock.h"

#include "BasicConfiguration.h"
#include "BestPheromoneForwardingPolicy.h"
#include "LinearPathReinforcementPolicy.h"
#include "ExponentialEvaporationPolicyMock.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClientMock::TestbedARAClientMock(Configuration& configuration) : TestbedARAClient(configuration) {

}

BasicConfiguration TestbedARAClientMock::getStandardConfiguration() const {
    float initialPhi = 5.0;
    float deltaPhi = 5.0;
    RoutingTable* routingTable = new RoutingTableMock();
    std::shared_ptr<TestbedPacketTrap> packetTrap = std::make_shared<TestbedPacketTrap>(routingTable);

    return BasicConfiguration(routingTable, new TestbedPacketFactory(15), 
        new ExponentialEvaporationPolicyMock(),
        new LinearPathReinforcementPolicy(deltaPhi),
        new BestPheromoneForwardingPolicy(routingTable),
        packetTrap,
        initialPhi);
}

void TestbedARAClientMock::receivePacket(Packet* packet, NetworkInterface* interface){
}

bool TestbedARAClientMock::handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface){
    return false;
}

void TestbedARAClientMock::deliverToSystem(const Packet* packet){

}

void TestbedARAClientMock::packetNotDeliverable(const Packet* packet){

}

void TestbedARAClientMock::sendPacket(Packet* packet){

}

TESTBED_NAMESPACE_END
