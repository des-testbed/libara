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

    void receivePacket(Packet* packet, NetworkInterface* interface);
    virtual bool handleBrokenLink(Packet* packet, std::shared_ptr<Address> nextHop, NetworkInterface* interface);

    void deliverToSystem(const Packet* packet);
    void packetNotDeliverable(const Packet* packet);

    unsigned char getCurrentEnergyLevel();

    // Mocking methods
    void setEnergy(unsigned char newEnergyLevel);

    PacketTrap* getPacketTrap();
    EnergyAwareRoutingTable* getRoutingTable();
    EARAPacketFactory* getPacketFactory();
    TimerMock* getEnergyDisseminationTimer();

    NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "DEFAULT");

private:
    unsigned char currentEnergyLevel;
};

ARA_NAMESPACE_END

#endif /* EARACLIENT_MOCK_H_ */
