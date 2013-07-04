/*
 * $FU-Copyright$
 */

#include "EARAClientMock.h"
#include "EARAPacketFactory.h"
#include "EnergyAwareRoutingTable.h"
#include "BestPheromoneForwardingPolicy.h"
#include "LinearPathReinforcementPolicy.h"
#include "testAPI/mocks/ExponentialEvaporationPolicyMock.h"
#include "testAPI/mocks/time/ClockMock.h"

ARA_NAMESPACE_BEGIN

EARAClientMock::EARAClientMock() {
    BasicEARAConfiguration configuration = getStandardConfiguration();
    initializeEARA(configuration);
    currentEnergyLevel = 255;
}

EARAClientMock::EARAClientMock(EARAConfiguration& configuration) {
    initializeEARA(configuration);
    currentEnergyLevel = 255;
}

BasicEARAConfiguration EARAClientMock::getStandardConfiguration() const {
    float initialPhi = 10.0;
    float deltaPhi = 5.0;
    int maxNrOfRouteDiscoveryRetries = 2;
    unsigned int routeDiscoveryTimeoutInMilliSeconds = 1000;
    unsigned int packetDeliveryDelayInMilliSeconds = 5;

    unsigned int maxEnergyValue = 100;
    float influenceOfMinimumEnergyValue = 3;
    unsigned int routeDiscoveryDelayInMilliSeconds = 2;

    EnergyAwareRoutingTable* routingTable = new EnergyAwareRoutingTable();

    return BasicEARAConfiguration(
        routingTable,
        new EARAPacketFactory(15),
        new ExponentialEvaporationPolicyMock(),
        new LinearPathReinforcementPolicy(deltaPhi),
        new BestPheromoneForwardingPolicy(routingTable),
        initialPhi,
        maxNrOfRouteDiscoveryRetries,
        routeDiscoveryTimeoutInMilliSeconds,
        packetDeliveryDelayInMilliSeconds,
        maxEnergyValue,
        influenceOfMinimumEnergyValue,
        routeDiscoveryDelayInMilliSeconds
    );
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

TimerMock* EARAClientMock::getRouteDiscoveryDelayTimer(AddressPtr source) {
    if (runningRouteDiscoveryDelayTimers.find(source) != runningRouteDiscoveryDelayTimers.end()) {
        return (TimerMock*) runningRouteDiscoveryDelayTimers[source];
    }
    else {
        return nullptr;
    }
}

ARA_NAMESPACE_END
