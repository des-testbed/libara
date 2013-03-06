/*
 * $FU-Copyright$
 */

#ifndef ARACLIENTMOCK_H_
#define ARACLIENTMOCK_H_

#include "AbstractARAClient.h"
#include "ForwardingPolicy.h"
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
    ARAClientMock();
    ~ARAClientMock();

    void receivePacket(Packet* packet, NetworkInterface* interface);

    void setEvaporationPolicy(EvaporationPolicy *policy);
    ForwardingPolicy* getForwardingPolicy();
    void updateRoutingTable(const Packet* packet, NetworkInterface* interface);
    void deliverToSystem(const Packet* packet);

    // Mocking methods

    PacketTrap* getPacketTrap();
    RoutingTable* getRoutingTable();
    NetworkInterfaceMock* createNewNetworkInterfaceMock(const std::string localAddressName = "DEFAULT");
    std::deque<const Packet*>* getDeliveredPackets();
    int getNumberOfReceivedPackets() ;
    std::deque<Pair<const Packet*, const NetworkInterface*>*> getReceivedPackets();

private:
    std::deque<NetworkInterfaceMock*> interfaceMocks;
    std::deque<const Packet*> deliveredPackets;
    std::deque<Pair<const Packet*, const NetworkInterface*>*> receivedPackets;

    ForwardingPolicy* forwardingPolicy;
    EvaporationPolicy* evaporationPolicy;
};

} /* namespace ARA */
#endif /* ARACLIENTMOCK_H_ */
