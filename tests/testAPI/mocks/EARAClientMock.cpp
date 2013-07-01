/*
 * $FU-Copyright$
 */

#include "ARAMacros.h"
#include "EARAClientMock.h"
#include "EARAPacketFactory.h"
#include "BasicEARAConfiguration.h"
#include "EnergyAwareRoutingTable.h"
#include "BestPheromoneForwardingPolicy.h"
#include "LinearPathReinforcementPolicy.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"
#include "testAPI/mocks/time/ClockMock.h"

ARA_NAMESPACE_BEGIN

EARAClientMock::EARAClientMock() {
    float initialPhi = 10.0;
    float deltaPhi = 5.0;
    BasicEARAConfiguration configuration = BasicEARAConfiguration(
            new ExponentialEvaporationPolicyMock(),
            new LinearPathReinforcementPolicy(deltaPhi),
            new BestPheromoneForwardingPolicy(),
            initialPhi
    );
    initializeEARA(configuration, new EnergyAwareRoutingTable(), new EARAPacketFactory(15));
    currentEnergyLevel = 255;
}

void EARAClientMock::receivePacket(Packet* packet, NetworkInterface* interface) {
    storeReceivedPacket(packet, interface);
    AbstractEARAClient::receivePacket(packet, interface);
}

bool EARAClientMock::handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    storeRouteFailurePacket(packetFactory->makeClone(packet), nextHop, interface);
    return AbstractEARAClient::handleBrokenLink(packet, nextHop, interface);
}

void EARAClientMock::deliverToSystem(const Packet* packet) {
    storeDeliveredPacket(packet);
}

void EARAClientMock::packetNotDeliverable(const Packet* packet) {
    storeUndeliverablePacket(packet);
}

unsigned char EARAClientMock::getCurrentEnergyLevel() {
    return currentEnergyLevel;
}

NetworkInterfaceMock* EARAClientMock::createNewNetworkInterfaceMock(const std::string localAddressName) {
    NetworkInterfaceMock* mock = AbstractClientMockBase::createNewNetworkInterfaceMock(localAddressName);
    addNetworkInterface(mock);
    return mock;
}

void EARAClientMock::setEnergy(unsigned char newEnergyLevel) {
    currentEnergyLevel = newEnergyLevel;
}

PacketTrap* EARAClientMock::getPacketTrap() {
    return packetTrap;
}

EnergyAwareRoutingTable* EARAClientMock::getRoutingTable() {
    return (EnergyAwareRoutingTable*) routingTable;
}

EARAPacketFactory* EARAClientMock::getPacketFactory() {
    return packetFactory;
}

ARA_NAMESPACE_END
