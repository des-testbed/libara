/*
 * $FU-Copyright$
 */

#ifndef ARACLIENTMOCK_H_
#define ARACLIENTMOCK_H_

#include "AbstractARAClient.h"
#include "Packet.h"
#include "NetworkInterfaceMock.h"
#include "PacketTrap.h"
#include "RoutingTable.h"

#include <string>
#include <deque>

namespace ARA {

/**
 * A ARAClientMock implements the abstract ARA Client and holds references to
 * other mocks for testing the routing table, packet trap and network interfaces.
 */
class ARAClientMock: public AbstractARAClient {
public:

    struct PacketInfo {
        const Packet* packet;
        std::shared_ptr<Address> nextHop;
        const NetworkInterface* interface;
    };

    ARAClientMock();
    ~ARAClientMock();

    void receivePacket(Packet* packet, NetworkInterface* interface);

    void deliverToSystem(const Packet* packet);
    virtual void handleRouteFailure(Packet* packet, std::shared_ptr<Address> nextHop, NetworkInterface* interface);
    void packetNotDeliverable(const Packet* packet);

    // Mocking methods

    PacketTrap* getPacketTrap();
    RoutingTable* getRoutingTable();
    NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "DEFAULT");
    std::deque<const Packet*>* getDeliveredPackets();

    int getNumberOfReceivedPackets();
    std::deque<Pair<const Packet*, const NetworkInterface*>*> getReceivedPackets();

    int getNumberOfRouteFailures();
    std::deque<PacketInfo> getRouteFailurePackets();

    int getNumberOfUndeliverablePackets();
    std::deque<const Packet*> getUndeliverablePackets();

private:
    std::deque<NetworkInterfaceMock*> interfaceMocks;
    std::deque<const Packet*> deliveredPackets;
    std::deque<Pair<const Packet*, const NetworkInterface*>*> receivedPackets;
    std::deque<PacketInfo> routeFailurePackets;
    std::deque<const Packet*> undeliverablePackets;
};

} /* namespace ARA */
#endif /* ARACLIENTMOCK_H_ */
