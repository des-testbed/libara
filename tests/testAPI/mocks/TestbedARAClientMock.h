/*
 * $FU-Copyright$
 */
#ifndef TESTBEDARACLIENTMOCK_H_
#define TESTBEDARACLIENTMOCK_H_

#include "Testbed.h"
#include "TestbedARAClient.h"
#include "TestbedPacketTrap.h"
#include "TestbedPacketFactory.h"

#include "ARAClientMock.h"
#include "RoutingTableMock.h"

/*
#include "Packet.h"
#include "TestbedNetworkInterfaceMock.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
*/

#include <string>

TESTBED_NAMESPACE_BEGIN

class TestbedARAClientMock: public TestbedARAClient, public ARAClientMock {
    public:
        TestbedARAClientMock(Configuration& configuration);
        BasicConfiguration getStandardConfiguration() const;

        void receivePacket(Packet* packet, NetworkInterface* interface);
        virtual bool handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface);

        void deliverToSystem(const Packet* packet);
        void packetNotDeliverable(const Packet* packet);

        void sendPacket(Packet* packet);

/*
    void setMaxHopCount(int n);
    double getInitialPhi() const;
    std::shared_ptr<PacketTrap> getPacketTrap();
    RoutingTable* getRoutingTable();
    NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "localhost");
    unsigned int getPacketDeliveryDelay() const;

    std::weak_ptr<Timer> getNeighborActivityTimer() const;
*/
    /**
     * Makes this client forget this neighbor (if he ever knew it)
     * This means all routing table entries are deleted and possibly existent activityTimes are discarded.
     */
    /*
    void forget(AddressPtr neighbor);

    std::weak_ptr<Timer> getPANTsTimer(AddressPtr destination);

    // make some methods public for testing purposes
    using AbstractARAClient::hasBeenReceivedEarlier;
    using AbstractARAClient::registerReceivedPacket;
    
    bool isPANTsTimerExpired(AddressPtr destination);
	bool isPANTsTimerType(AddressPtr destination, TimerType type);
    bool isPANTsTimerRunning(AddressPtr destination);
    void expirePANTsTimer(AddressPtr destination);
    */
};

TESTBED_NAMESPACE_END

#endif /* ARACLIENTMOCK_H_ */
