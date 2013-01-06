/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

#ifndef ABSTRACTARACLIENT_H_
#define ABSTRACTARACLIENT_H_

#include "Address.h"
#include "NextHop.h"
#include "NetworkInterface.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "LinkedList.h"
#include <unordered_map>
#include <unordered_set>

namespace ARA {

//TODO fix the visibility: most of the methods should be protected instead of public

class AbstractARAClient {
public:
    AbstractARAClient();
    virtual ~AbstractARAClient();

    /**
     * TODO write some documentation
     */
    virtual NextHop* getNextHop(const Packet* packet) = 0;

    /**
     * This method is called each a time a new packet is received over the
     * given interface.
     * It's is responsible for updating the routing table so a route to the
     * packets source is known in the future.
     *
     * Note: This method is not called on duplicate packets (which trigger a DUPLICATE_WARNING).
     */
    virtual void updateRoutingTable(const Packet* packet, NetworkInterface* interface) = 0;

    /**
     * The packet should be directed to this node and must be delivered to the local system.
     */
    virtual void deliverToSystem(const Packet* packet) = 0;

    /**
     * Registers a new NetworkInterface at this client.
     */
    void addNetworkInterface(NetworkInterface* newInterface);

    /**
     * Returns a specific NetworkInterface by index.
     */
    NetworkInterface* getNetworkInterface(unsigned int index);

    /**
     * Returns the amount of registered network interfaces of this client.
     */
    unsigned int getNumberOfNetworkInterfaces();

    /**
     * Sends the packet to the packets destination.
     *
     * If the packet is deliverable (e.g there is at least one route known
     * in the routing table), the next hop is calculated in AbstractARAClient::getNextHop()
     * If there is no known route to the packet destination a FANT is generated and send
     * according to the ARA algorithm.
     */
    void sendPacket(const Packet* packet);

    /**
     * Receive a Packet over the given NetworkInterface. The packet will be
     * processed according to the Ant Routing Algorithm (ARA).
     *
     * TODO write some more documentation
     */
    void receivePacket(const Packet* packet, NetworkInterface* interface);

    //TODO AbstractARAClient::broadCast(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    void broadCast(const Packet* packet);
    //TODO AbstractARAClient::getNextSequenceNumber(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    unsigned int getNextSequenceNumber();
    //TODO AbstractARAClient::hasBeenReceivedEarlier(...) should be protected. It is not because else the AbstractARAClientTest can not see this.. :(
    bool hasBeenReceivedEarlier(const Packet* packet);
    //TODO AbstractARAClient::registerReceivedPacket(...) should be private. It is not because else the AbstractARAClientTest can not see this.. :(
    void registerReceivedPacket(const Packet* packet);

protected:

    LinkedList<NetworkInterface> interfaces;
    PacketTrap packetTrap;
    RoutingTable routingTable;

private:
    unsigned int nextSequenceNumber = 1;
    std::unordered_map<Address*, std::unordered_set<unsigned int>*, AddressHash, AddressPredicate> lastReceivedPackets;

    void sendDuplicateWarning(Address* recipient, NetworkInterface* interface);
    void handlePacket(const Packet* packet);
    void handleDataPacket(const Packet* packet);
    void handleAntPacket(const Packet* packet);
    void handleAntPacketForThisNode(const Packet* packet);
    bool isDirectedToThisNode(const Packet* packet);
};

} /* namespace ARA */
#endif /* ABSTRACTARACLIENT_H_ */
