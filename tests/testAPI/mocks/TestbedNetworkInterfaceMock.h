/*
 * $FU-Copyright$
 */

#ifndef TESTBED_NETWORK_INTERFACE_MOCK_H_
#define TESTBED_NETWORK_INTERFACE_MOCK_H_

#include "Testbed.h"
#include "TestbedPacketFactory.h"
#include "TestbedNetworkInterface.h"

#include "ReliableNetworkInterface.h"
#include "AbstractNetworkClient.h"
#include "Packet.h"
#include "Pair.h"
#include "AddressMock.h"

#include <deque>

TESTBED_NAMESPACE_BEGIN

class TestbedNetworkInterfaceMock: public TestbedNetworkInterface {
    public:
        TestbedNetworkInterfaceMock(dessert_meshif_t* interface, AbstractNetworkClient* client);
        TestbedNetworkInterfaceMock(dessert_meshif_t* interface, const std::string interfaceName, AbstractNetworkClient* client);
/*
    NetworkInterfaceMock(const std::string interfaceName, AbstractNetworkClient* client);
    NetworkInterfaceMock(const std::string interfaceName, const std::string localAddressName, AbstractNetworkClient* client);
    ~NetworkInterfaceMock();
    void doSend(const Packet* packet, std::shared_ptr<Address> recipient);
    void broadcast(const Packet* packet);
    bool equals(NetworkInterface* otherInterface);

    std::string getName();
    std::deque<Pair<const Packet*, std::shared_ptr<Address>>*>* getSentPackets();
    unsigned int getNumberOfSentPackets();
    bool hasPacketBeenSent(Packet* packet);
    bool hasPacketBeenBroadCasted(Packet* packet);

    int getNrOfUnacknowledgedPackets() const;
    int getNrOfRunningTimers() const;

*/
        static std::string DEFAULT_INTERFACE_NAME;
        static int DEFAULT_ACK_TIMEOUT;

    private:
        std::string name;
//    std::deque<Pair<const Packet*, std::shared_ptr<Address>>*> sentPackets;
//    std::deque<const Packet*> broadcastedPackets;
};

TESTBED_NAMESPACE_END

#endif /* TESTBED_NETWORK_INTERFACE_MOCK_H_ */
