/*
 * $FU-Copyright$
 */

#ifndef NETWORKINTERFACEMOCK_H_
#define NETWORKINTERFACEMOCK_H_

#include "ARAMacros.h"
#include "ReliableNetworkInterface.h"
#include "AbstractNetworkClient.h"
#include "Packet.h"
#include "Pair.h"
#include "AddressMock.h"

#include <deque>

ARA_NAMESPACE_BEGIN

class NetworkInterfaceMock: public ARA::ReliableNetworkInterface {
public:
    NetworkInterfaceMock(AbstractNetworkClient* client);
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

    static std::string DEFAULT_INTERFACE_NAME;
    static AddressPtr DEFAULT_LOCAL_ADDRESS;
    static AddressPtr DEFAULT_BROADCAST_ADDRESS;
    static int DEFAULT_ACK_TIMEOUT;

private:
    std::string name;
    std::deque<Pair<const Packet*, std::shared_ptr<Address>>*> sentPackets;
    std::deque<const Packet*> broadcastedPackets;
};

ARA_NAMESPACE_END

#endif /* NETWORKINTERFACEMOCK_H_ */
