/*
 * $FU-Copyright$
 */

#ifndef EARACLIENT_MOCK_H_
#define EARACLIENT_MOCK_H_

#include "ARAMacros.h"
#include "AbstractEARAClient.h"
#include "AbstractClientMockBase.h"
#include "BasicEARAConfiguration.h"
#include "Packet.h"
#include "NetworkInterfaceMock.h"
#include "PacketTrap.h"
#include "EnergyAwareRoutingTable.h"
#include "testAPI/mocks/time/TimerMock.h"

#include <string>

ARA_NAMESPACE_BEGIN

/**
 * A EARAClientMock implements the abstract EARA Client and holds references to
 * other mocks for testing the routing table, packet trap and network interfaces.
 */
class EARAClientMock: public AbstractEARAClient, public AbstractClientMockBase {
public:
    EARAClientMock();
    EARAClientMock(EARAConfiguration& configuration);
    BasicEARAConfiguration getStandardConfiguration() const;

    virtual void receivePacket(Packet* packet, NetworkInterface* interface);
    bool handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface);

    virtual void deliverToSystem(const Packet* packet);
    virtual void packetNotDeliverable(const Packet* packet);

    virtual unsigned int getCurrentEnergyLevel();

    // Mocking methods
    void setEnergy(unsigned int newEnergyLevel);

    std::shared_ptr<PacketTrap> getPacketTrap();
    EnergyAwareRoutingTable* getRoutingTable();
    EARAPacketFactory* getPacketFactory();
    TimerMock* getEnergyDisseminationTimer();

    bool isRouteDiscoveryDelayTimerExpired(AddressPtr source);
	bool isRouteDiscoveryDelayTimerType(AddressPtr destination, TimerType type);
    bool isRouteDiscoveryDelayTimerRunning(AddressPtr destination);
    void expireRouteDiscoveryDelayTimer(AddressPtr destination);

    NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "DEFAULT");

    // make some methods public for testing purposes
    using AbstractEARAClient::normalizeEnergyValue;
private:
    unsigned int currentEnergyLevel;
};

ARA_NAMESPACE_END

#endif /* EARACLIENT_MOCK_H_ */
