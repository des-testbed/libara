/*
 * $FU-Copyright$
 */

#ifndef _NETWORK_INTERFACE_H_
#define _NETWORK_INTERFACE_H_

#include "Testbed.h"
#include "AbstractNetworkClient.h"
#include "ReliableNetworkInterface.h"

#include <map>
#include <mutex>

TESTBED_NAMESPACE_BEGIN

/**
 *
 */
class TestbedNetworkInterface : public ReliableNetworkInterface {
    public:
        TestbedNetworkInterface(std::string interfaceName, AbstractNetworkClient* client, std::shared_ptr<TestbedAddress> localAddress, std::shared_ptr<TestbedAddress> broadcastAddress, PacketFactory* packetFactory, int ackTimeoutInMicroSeconds);

        virtual ~TestbedNetworkInterface(){};

        /**
         *
         */
        void send(const Packet* packet, AddressPtr recipient);
        /**
         *
         */
        void receive(Packet* packet);
        /**
         *
         */
        bool equals(NetworkInterface* otherInterface);

        /**
         *
         */
        std::string getStatistics();

        /**
         *
         */
        std::string getInterfaceName();

        /**
         * @see ReliableNetworkInterface::timerHasExpired(std::weak_ptr<Timer> ackTimer)
         */
        void timerHasExpired(std::weak_ptr<Timer> ackTimer);

    protected:
        void doSend(const Packet* packet, std::shared_ptr<Address> recipient);

    private:
        /**
         *
         */
        void packetCounter(char type, bool isReceivedPacket);

        std::mutex unacknowledgedPacketsMutex;
        std::mutex acknowledgmentTimerMutex;
        std::mutex receiveMutex;
        std::mutex sendMutex;

        /// the total number of received packets
        unsigned long numberOfReceivedPackets;
        /// the total number of sent packets
        unsigned long numberOfSentPackets;

        /// we store the number of received packets per packet type in a map
        std::map<std::string, unsigned long> receiveStatistics;
        /// we store the number of sent packets per packet type in a map
        std::map<std::string, unsigned long> sentStatistics;

        std::string interfaceName;
};

TESTBED_NAMESPACE_END

#endif // _NETWORK_INTERFACE_H_
