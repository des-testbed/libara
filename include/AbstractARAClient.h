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

#include "Logger.h"
#include "Address.h"
#include "NextHop.h"
#include "NetworkInterface.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "ForwardingPolicy.h"

#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <memory>
#include <string>
#include <sstream>

namespace ARA {

//TODO fix the visibility: most of the methods should be protected instead of public

class AbstractARAClient {
public:
    AbstractARAClient();
    virtual ~AbstractARAClient();

    /**
     * This method is called each a time a new packet is received over the
     * given interface.
     * It's is responsible for updating the routing table so a route to the
     * packets source is known in the future.
     *
     * Note: This method is not called on duplicate packets (which trigger a DUPLICATE_ERROR).
     */
    virtual void updateRoutingTable(const Packet* packet, NetworkInterface* interface) = 0;

    /**
     * The packet should be directed to this node and must be delivered to the local system.
     */
    virtual void deliverToSystem(const Packet* packet) = 0;

    /**
     * Sets a logger for this ARA client.
     *
     * This logger will be used to log messages during the routing algorithm.
     * It will be deleted in the destructor of this client.
     */
    void setLogger(Logger* logger);

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
     * in the routing table), the next hop is calculated via the current forwarding policy.
     * If there is no known route to the packet destination a FANT is generated and send
     * according to the ARA algorithm.
     *
     * Note: The packet instance may be deleted after this method so we need to
     * clone the packet if we want to persist it in memory.
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

    /// The method initializes the pheromone value of a link
    void initializePheromone(const Packet* packet, NetworkInterface* interface);

    ///
    virtual void setEvaporationPolicy(EvaporationPolicy *policy) = 0;

protected:

    std::deque<NetworkInterface*> interfaces;
    RoutingTable routingTable;
    PacketTrap* packetTrap;

    /// The member specifies the initial level 
    double initialPhi;

    /**
     * This method is called to retrieve an instance of ForwardingPolicy
     * each time the next hop for a given destination has to be determined.
     *
     * Note: If the forwarding policy is static (i.e. does never change), the
     * implementation should store the forwarding policy as a member and just
     * return a pointer to it instead of creating a new instance each time
     * this method is called.
     */
    virtual ForwardingPolicy* getForwardingPolicy() = 0;

    /**
     * Checks if a logger has been assigned to this ARA client and if so
     * delegates the call to it with Logger::LEVEL_DEBUG.
     * The optional varargs are handled as parameters to the logMessage string.
     *
     * If no logger has been set via setLogger() nothing happens.
     */
    void logMessage(const std::string &logMessage, Logger::Level level, ...) const;

    /**
     * Logs with level debug.
     * See AbstractARAClient::logMessage for more details.
     */
    void logDebug(const std::string &logMessage, ...) const;

private:
    Logger* logger = nullptr;
    unsigned int nextSequenceNumber = 1;
    std::unordered_map<std::shared_ptr<Address>, std::unordered_set<unsigned int>*, AddressHash, AddressPredicate> lastReceivedPackets;

    NextHop* getNextHop(const Packet* packet);
    void sendDuplicateWarning(const Packet* packet, NetworkInterface* interface);
    void handlePacket(const Packet* packet, NetworkInterface* interface);
    void handleDataPacket(const Packet* packet);
    void handleAntPacket(const Packet* packet);
    void handleAntPacketForThisNode(const Packet* packet);
    void handleDuplicateErrorPacket(const Packet* packet, NetworkInterface* interface);
    bool isDirectedToThisNode(const Packet* packet) const;
    bool hasBeenSentByThisNode(const Packet* packet) const;

};

} /* namespace ARA */
#endif /* ABSTRACTARACLIENT_H_ */
