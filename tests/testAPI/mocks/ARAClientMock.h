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
#include "testAPI/mocks/time/TimerMock.h"

#include <string>

ARA_NAMESPACE_BEGIN

class NetworkInterfaceMock;
typedef std::shared_ptr<TimerMock> TimerMockPtr;

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
    virtual bool handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface);

    void deliverToSystem(const Packet* packet);
    void packetNotDeliverable(const Packet* packet);

    void setMaxHopCount(int n);
    double getInitialPhi() const;
    PacketTrap* getPacketTrap();
    RoutingTable* getRoutingTable();
    NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "localhost");
    unsigned int getPacketDeliveryDelay() const;

    TimerMockPtr getNeighborActivityTimer() const;

    /**
     * Makes this client forget this neighbor (if he ever knew it)
     * This means all routing table entries are deleted and possibly existent activityTimes are discarded.
     */
    void forget(AddressPtr neighbor);

    TimerMockPtr getPANTsTimer(AddressPtr destination);

    // make some methods public for testing purposes
    using AbstractARAClient::hasBeenReceivedEarlier;
    using AbstractARAClient::registerReceivedPacket;
};

ARA_NAMESPACE_END

#endif /* ARACLIENTMOCK_H_ */
