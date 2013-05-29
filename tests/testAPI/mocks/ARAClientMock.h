/*
 * $FU-Copyright$
 */

#ifndef ARACLIENTMOCK_H_
#define ARACLIENTMOCK_H_

#include "TestMacros.h"
#include "AbstractARAClient.h"
#include "AbstractClientMockBase.h"
#include "Configuration.h"
#include "Packet.h"
#include "NetworkInterfaceMock.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "BasicConfiguration.h"

#include <string>

ARA_NAMESPACE_BEGIN

class NetworkInterfaceMock;

/**
 * A ARAClientMock implements the abstract ARA Client and holds references to
 * other mocks for testing the routing table, packet trap and network interfaces.
 */
class ARAClientMock: public AbstractARAClient, public AbstractClientMockBase {
public:
    ARAClientMock();
    ARAClientMock(Configuration& configuration);

    BasicConfiguration getStandardConfiguration() const;

    void receivePacket(Packet* packet, NetworkInterface* interface);
    virtual void handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface);

    void deliverToSystem(const Packet* packet);
    void packetNotDeliverable(const Packet* packet);

    void setMaxHopCount(int n);
    double getInitialPhi() const;
    PacketTrap* getPacketTrap();
    RoutingTable* getRoutingTable();
    NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "DEFAULT");
    unsigned int getPacketDeliveryDelay() const;

    Timer* getNeighborActivityTimer() const;
    Timer* getPANTsTimer(AddressPtr destination) const;
};

ARA_NAMESPACE_END

#endif /* ARACLIENTMOCK_H_ */
