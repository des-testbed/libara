/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_NETWORK_CLIENT_H_
#define ABSTRACT_NETWORK_CLIENT_H_

#include "ARAMacros.h"
#include "NetworkInterface.h"
#include "PacketFactory.h"
#include "PacketTrap.h"
#include "RoutingTable.h"
#include "Configuration.h"
#include "Packet.h"

#include <string>
#include <deque>
#include <cassert>

ARA_NAMESPACE_BEGIN

/**
 * The AbstractNetworkClient bundles general functionality for any Ad-Hoc network client.
 * It contains a variable number of NetworkInterfaces, a RoutingTable, a PacketTrap, a PacketFactory
 * and a Logger.
 *
 * There is not much logic implemented in this class, because this does only serve as base class to
 * more sophisticated network clients which route packets through the network.
 */
class AbstractNetworkClient {

public:
    /**
     * The standard virtual destructor of this abstract class.
     */
    virtual ~AbstractNetworkClient();

    /**
     * Somehow sends the packet to the packets destination.
     */
    virtual void sendPacket(Packet* packet) = 0;

    /**
     * Receives and processes a Packet.
     */
    virtual void receivePacket(Packet* packet, NetworkInterface* interface) = 0;

    /**
     * The packet should be directed to this node and must be delivered to the local system.
     * Please note that this method is responsible for deleting the given packet (or delegating
     * this responsibility to another method)
     */
    virtual void deliverToSystem(const Packet* packet) = 0;

    /**
     * This method is called if the route discovery is unsuccessful and not route to the packets
     * destination can be established. The task of this method is to notify the upper layers
     * about this event and delete the packet.
     */
    virtual void packetNotDeliverable(const Packet* packet) = 0;

    /**
     * This method is called each time packet can not be delivered to a specific next hop address.
     * This is the case if this client never receives an acknowledgment in the timeout period
     * and has tried too many times.
     */
    virtual bool handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) = 0;

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
     * Returns the packet factory of this node.
     */
    virtual PacketFactory* getPacketFactory() const;

    /**
     * Assigns a new RoutingTable object to this NetworkClient
     */
    void setRoutingTable(RoutingTable *routingTable);

    /**
     * Sends the given packet over all available interfaces of this node.
     * The sender is automatically set to the corresponding interface address.
     * The previous hop will be the original sender of the packet.
     */
    virtual void broadCast(Packet* packet);

    /**
     * Returns the next sequence number and advances the sequence number counter by one.
     */
    unsigned int getNextSequenceNumber();

protected:
    /**
     * Checks if the destination of the given packet equals the local address
     * of one of this nodes interfaces.
     */
    bool isDirectedToThisNode(const Packet* packet) const;

    /**
     * Checks if the source of the given packet equals the local address
     * of one of this nodes interfaces.
     */
    bool hasBeenSentByThisNode(const Packet* packet) const;

    /**
     * Checks if the given address equals the local address of one of
     * this nodes interfaces.
     */
    virtual bool isLocalAddress(AddressPtr address) const;

protected:
    std::deque<NetworkInterface*> interfaces;
    PacketFactory* packetFactory;
    RoutingTable* routingTable;
    std::shared_ptr<PacketTrap> packetTrap;

private:
    unsigned int nextSequenceNumber = 1;
};

ARA_NAMESPACE_END

#endif /* ABSTRACT_NETWORK_CLIENT_H_ */
