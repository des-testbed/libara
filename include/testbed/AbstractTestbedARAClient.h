/*
 * $FU-Copyright$
 */

#ifndef _ABSTRACT_TESTBED_ARA_CLIENT_H_
#define _ABSTRACT_TESTBED_ARA_CLIENT_H_

#include "Testbed.h"
#include "AbstractARAClient.h"

TESTBED_NAMESPACE_BEGIN

class AbstractTestbedARAClient : public AbstractARAClient {
    public:

        /**
         * The standard virtual destructor of this abstract class.
         */
        ~AbstractTestbedARAClient();

        /**
         * Sends the packet to the packets destination.
         *
         * If the packet is deliverable (e.g there is at least one route known
         * in the routing table), the next hop is calculated via the current
         * forwarding policy. If there is no known route to the packet destination
         * a FANT is generated and send according to the Abstract Ant Routing
         * Algorithm (ARA).
         *
         * @param packet the Packet to be send. Please note that the packet will be
         *        deleted by the AbstractARAClient when it is no longer needed.
         */
        void sendPacket(Packet* packet);

        /**
         * Receive a Packet over the given NetworkInterface. The packet will be
         * processed according to the Ant Routing Algorithm (ARA).
         *
         * @param packet the received Packet. Please note that the packet will be
         *        deleted by the AbstractARAClient when it has been fully processed.
         */
        void receivePacket(Packet* packet, NetworkInterface* interface);

        /**
         * This method is called each time packet can not be delivered to a specific next hop address.
         * This is the case if this client never receives an acknowledgment in the timeout period
         * and has tried too many times.
         */
        bool handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface);
        void timerHasExpired(Timer* responsibleTimer);

    protected:
        /**
         * Handles a packet depending on its type. This method is protected virtual to enable
         * other ARA implementations (like EARA) to override it to add new packet types.
         */
        void handlePacket(Packet* packet, NetworkInterface* interface);
        void handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface);
        void startNewRouteDiscovery(Packet* packet);
        void handleNonSourceRouteDiscovery(Packet* packet);
        void handlePacketWithZeroTTL(Packet* packet);
};

TESTBED_NAMESPACE_END

#endif // _ABSTRACT_TESTBED_ARA_CLIENT_H_
