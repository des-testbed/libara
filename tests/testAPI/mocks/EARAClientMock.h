/*
 * $FU-Copyright$
 */

#ifndef EARACLIENT_MOCK_H_
#define EARACLIENT_MOCK_H_

#include "AbstractEARAClient.h"
#include "AbstractClientMockBase.h"
#include "Packet.h"
#include "NetworkInterfaceMock.h"
#include "PacketTrap.h"
#include "EnergyAwareRoutingTable.h"

#include <string>

namespace ARA {

/**
 * A EARAClientMock implements the abstract EARA Client and holds references to
 * other mocks for testing the routing table, packet trap and network interfaces.
 */
class EARAClientMock: public AbstractEARAClient, public AbstractClientMockBase {
public:
    EARAClientMock();

    void receivePacket(Packet* packet, NetworkInterface* interface);
    virtual void handleRouteFailure(Packet* packet, std::shared_ptr<Address> nextHop, NetworkInterface* interface);

    void deliverToSystem(const Packet* packet);
    void packetNotDeliverable(const Packet* packet);

    PacketTrap* getPacketTrap();
    EnergyAwareRoutingTable* getRoutingTable();
    NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "DEFAULT");
};

} /* namespace ARA */
#endif /* EARACLIENT_MOCK_H_ */
