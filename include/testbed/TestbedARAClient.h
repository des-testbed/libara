/*
* $FU-Copyright$
 */

#ifndef _TESTBED_ARA_CLIENT_H_
#define _TESTBED_ARA_CLIENT_H_

#include "Testbed.h"
#include "TestbedAddress.h"
#include "TestbedNetworkInterface.h"
#include "AbstractARAClient.h"

#include <mutex>

TESTBED_NAMESPACE_BEGIN

class TestbedARAClient : public AbstractARAClient {
    public:
        /**
         * Creates a TestbedARAClient and runs initialize() with given configuration.
         */
        TestbedARAClient(Configuration &config);

        /**
         * The standard virtual destructor of this abstract class.
         */
        ~TestbedARAClient();

        /**
         * Sends the packet to the packets destination.
         *
         * If the packet is deliverable (e.g there is at least one route known
         * in the routing table), the next hop is calculated via the current
         * forwarding policy. If there is no known route to the packet destination,
         * a new route discovery begins.
         *
         * @param packet the Packet to be send. Please note that the packet will be
         *        deleted by the AbstractARAClient when it is no longer needed.
         */
        void sendPacket(Packet* packet);

        /**
          Receive a Packet over the given NetworkInterface. The packet will be
         * processed according to the Ant Routing Algorithm (ARA).
         *
         * @param packet the received Packet. Please note that the packet will be
         *        deleted by the AbstractARAClient when it has been fully processed.
         */
        void receivePacket(Packet* packet, ARA::NetworkInterface* interface);

        /**
         * The packet should be directed to this node and must be delivered to the local system.
         * Please note that this method is responsible for deleting the given packet (or delegating
         * this responsibility to another method)
         */
        void deliverToSystem(const Packet* packet);

        /**
         * This method is called if the route discovery is unsuccessful and not route to the packets
         * destination can be established. The task of this method is to notify the upper layers
         * about this event and delete the packet.
         */
        void packetNotDeliverable(const Packet* packet);

        void handleExpiredRouteDiscoveryTimer(std::weak_ptr<Timer> routeDiscoveryTimer);

        void handleExpiredDeliveryTimer(std::weak_ptr<Timer> deliveryTimer);
    
        void handleExpiredPANTTimer(std::weak_ptr<Timer> pantTimer);

        void stopRouteDiscoveryTimer(AddressPtr destination);

        std::string routingTableToString();

        TestbedNetworkInterface* getTestbedNetworkInterface(std::shared_ptr<TestbedAddress> address);

    protected:
        /**
         * This method initializes all of the network interfaces available on the device.
         */
        void initializeNetworkInterfaces();

        /**
         * Extends AbstractNetworkClient isLocalAddress to check the MAC address of the TAP interface as well.
         */
        virtual bool isLocalAddress(AddressPtr address) const;

        /**
         * Overrides AbstractARAClient's sendFANT method to prevent duplicate route establishment
         */
        void broadcastFANT(AddressPtr destination);

        TestbedAddressPtr tapAddress;


    private:
	/**
	 * This mutex protects the access to the map which holds the delivery timers.
	 */
	std::mutex deliveryTimerMutex;

	/**
	 * This mutex protects the access to the map which holds the route discovery timers.
	 */
	std::mutex routeDiscoveryTimerMutex;

	/**
	 * This mutex protects the access to the map which holds the pant timers.
	 */
	std::mutex pantTimerMutex;
};

TESTBED_NAMESPACE_END

#endif // _TESTBED_ARA_CLIENT_H_
