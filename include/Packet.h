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

#ifndef PACKET_H_
#define PACKET_H_

#include "Address.h"
#include "PacketType.h"
#include <stddef.h>
#include <memory>

namespace ARA {

/**
 * Packets encapsulate a payload that has to be transmitted from
 * a source node to a destination node.
 */
class Packet {
public:
    Packet(std::shared_ptr<Address> source, std::shared_ptr<Address> destination, std::shared_ptr<Address> sender, char type, unsigned int seqNr, const char* payload=NULL, unsigned int payloadSize=0, unsigned int hopCount = 1);
    Packet(std::shared_ptr<Address> source, std::shared_ptr<Address> destination, std::shared_ptr<Address> sender, char type, unsigned int seqNr, unsigned int hopCount);
    Packet(std::shared_ptr<Address> source, std::shared_ptr<Address> destination, char type, unsigned int seqNr);
    virtual ~Packet();

    /**
     * Returns the address of the node that has initially created and send a packet
     * with this payload and sequence number directed at the destination node.
     * It must not be confused with the sender of this packet which corresponds
     * to the node from which this packet has been received (layer 2).
     *
     * @see Packet::getDestination()
     * @see Packet::getSender()
     */
    std::shared_ptr<Address> getSource() const;

    /**
     * Returns the address of the node to whom the payload of this packet is directed.
     * It will not necessarily be the layer 2 receiver of this packet if the packet is
     * relayed through the network.
     *
     * @see Packet::getSource()
     * @see Packet::getSender()
     */
    std::shared_ptr<Address> getDestination() const;

    /**
     * Returns the address of the node from which this packet has been received (layer 2).
     * This is not necessarily the original source of the packets information which is
     * returned by Packet::getSource().
     *
     * @see Packet::getSource()
     * @see Packet::getDestination()
     */
    std::shared_ptr<Address> getSender() const;

    /**
     * Assigns a new sender to this packet.
     */
    void setSender(std::shared_ptr<Address> newSender);

    /**
     * Returns the type of this packet as an integer. The integer mapping is defined in
     * the PacketType enum class.
     */
    char getType() const;

    /**
     * Returns the sequence number of this packet. It has been set once by the packets
     * source and will not be modified on the packets path through the network.
     */
    unsigned int getSequenceNumber() const;

    /**
     * Calculates and returns the has value of this object based on the hash value
     * of its source address and the sequence number.
     * This method is used to put packets in HashSets from where they can be retrieved
     * in O(1) time like in the packet trap.
     */
    size_t getHashValue() const;

    /**
     * Returns the number of links this packet has been send over or respectively
     * the number of nodes this packet has been send by to arrive at the current node.
     * Note: After a packet has been received from another node this will be at least 1.
     */
    unsigned int getHopCount() const;

    const char* getPayload() const;

    unsigned int getPayloadLength() const;

    /**
     * Two packets are defined to be equal if they have the same source and sequence number
     * TODO is it really okay to define equality this way? What about the payload, destination and type?
     */
    virtual bool equals(const Packet* otherPacket) const;

    virtual bool operator==(Packet& other) {
        return this->equals(&other);
    }

    bool isDataPacket() const {
        return PacketType::isDataPacket(this->type);
    }

    bool isAntPacket() const {
        return PacketType::isAntPacket((this->type));
    }

    /**
     * Sets the hop count of this packet to a specific value.
     *
     * @see Packet::increaseHopCount()
     */
    void setHopCount(unsigned int newValue);

    /**
     * Increases the hop count of this packet by one.
     *
     * @see Packet::setHopCount()
     */
    void increaseHopCount();

    virtual Packet* clone() const;

    /**
     * Creates a new FANT based on this packet. The FANT inherits all the
     * addresses of this packet. The hop count is also replicated.
     *
     * Note: The result of this method is a newly created object which must be
     * deleted later by the calling class.
     */
	virtual Packet* createFANT(unsigned int sequenceNumber) const;

	/**
	 * Creates a new BANT based on this packet. This BANT has the destination of
	 * this packet as its source and the destination of this as its source.
	 * The hop count will be set to 0. The sequence number of the BANT is
	 * given as argument of this method.
	 *
	 * Note: The result of this method is a newly created object which must be
	 * deleted later by the calling class.
	 */
	virtual Packet* createBANT(unsigned int sequenceNumber) const;

	/**
     * Creates a new DUPLICATE_WARNING packet based on the information in this
     * packet. The DUPLICATE_WARNING inherits all the addresses of this packet.
     * The hop count is incremented.
     *
     * Note: The result of this method is a newly created object which must be
     * deleted later by the calling class.
     */
    virtual Packet* createDuplicateWarning() const;

protected:
    std::shared_ptr<Address> source;
    std::shared_ptr<Address> destination;
    std::shared_ptr<Address> sender;
    char type;
    unsigned int seqNr;
    const char* payload;
    unsigned int payloadSize;
    unsigned int hopCount;

friend class PacketPredicate;
};

/**
 * This Functor is needed for std::unordered_map (hashset implementation)
 */
struct PacketHash {
    size_t operator()(const Packet* packet) const {
        return packet->getHashValue();
    }
};

/**
 * This Functor is needed for std::unordered_set (hashset implementation)
 */
struct PacketPredicate {
    size_t operator()(const Packet* packet1, const Packet* packet2) const {
        return packet1->seqNr == packet2->seqNr &&
               packet1->source->equals(packet2->source);
    }
};

} /* namespace ARA */
#endif // PACKET_H_
