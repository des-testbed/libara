/*
 * $FU-Copyright$
 */

#ifndef PACKET_H_
#define PACKET_H_

#include "ARAMacros.h"
#include "Address.h"
#include "PacketType.h"

#include <stddef.h>
#include <memory>
#include <string>

ARA_NAMESPACE_BEGIN

/**
 * Packets encapsulate a payload that has to be transmitted from
 * a source node to a destination node.
 */
class Packet {
public:
    Packet(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0);
    Packet(AddressPtr source, AddressPtr destination, char type, unsigned int seqNr, int ttl);
    virtual ~Packet();

    /**
     * Returns the address of the node that has initially created and send a packet
     * with this payload and sequence number directed at the destination node.
     * It must not be confused with the sender of this packet which corresponds
     * to the node from which this packet has been received (layer 2).
     *
     * @see Packet::getDestination()
     * @see Packet::getSender()
     * @see Packet::getPenultimateHop()
     */
    AddressPtr getSource() const;

    /**
     * Returns the address of the node to whom the payload of this packet is directed.
     * It will not necessarily be the layer 2 receiver of this packet if the packet is
     * relayed through the network.
     *
     * @see Packet::getSource()
     * @see Packet::getSender()
     * @see Packet::getPenultimateHop()
     */
    AddressPtr getDestination() const;

    /**
     * Returns the address of the node from which this packet has been received (layer 2).
     * This is not necessarily the original source of the packets information which is
     * returned by Packet::getSource().
     *
     * @see Packet::getSource()
     * @see Packet::getDestination()
     * @see Packet::getPenultimateHop()
     */
    AddressPtr getSender() const;

    /**
     * Returns the address of the node from which the sender has received this packet.
     * This may be nullptr if the sender equals the source.
     *
     * @see Packet::getSource()
     * @see Packet::getDestination()
     * @see Packet::getSender()
     */
    AddressPtr getPreviousHop() const;

    /**
     * Returns the null-terminated string representation of the address of the source.
     * This is only a shortcut to getSource()->toString() for the convenience of the
     * developer.
     *
     * @see Packet::getSource()
     */
    std::string getSourceString() const {
        return getSource()->toString();
    }

    /**
     * Returns the null-terminated string representation of the sender address.
     * This is only a shortcut to getSender()->toString() for the convenience of the
     * developer.
     *
     * @see Packet::getSender()
     */
    std::string getSenderString() const {
        return getSender()->toString();
    }

    /**
     * Returns the null-terminated string representation of the destination address.
     * This is only a shortcut to getDestination()->toString() for the convenience of the
     * developer.
     *
     * @see Packet::getDestination()
     */
    std::string getDestinationString() const {
        return getDestination()->toString();
    }

    /**
     * Assigns a new sender to this packet.
     * This returns a copy to self which makes chaining methods pretty ease.
     * Example: Packet* packet = packetFactory->makeFANT(source, destination, seqNr)->setSender(foo);
     */
    Packet* setSender(AddressPtr newSender);

    /**
     * Assigns a new penultimate hop to this packet.
     * This returns a copy to self which makes chaining methods pretty ease.
     * Example: Packet* packet = packetFactory->makeFANT(source, destination, seqNr)->setSender(foo);
     */
    Packet* setPreviousHop(AddressPtr newPreviousHop);

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
     * Returns the time to live (TTL) of this packet.
     * This represents the maximum number of times that this packet can be relayed.
     * Note: The number of hops this packet has traveled so far can be calculated by
     * subtracting the TTL from the globally configured maximum number of hops.
     */
    unsigned int getTTL() const;

    /**
     * Increases the TTL value by 1.
     * This may be necessary in route failure handling when we must make sure, the TTL
     * value is not decreased multiple times.
     */
    void increaseTTL();

    /**
     * Decreases the TTL value by the `times' parameter.
     * This is only used for testing.
     */
    void decreaseTTL(int times=1);

    const char* getPayload() const;

    void** getPayloadPointer() const;

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

protected:
    AddressPtr source;
    AddressPtr destination;
    AddressPtr sender;
    AddressPtr previousHop;
    char type;
    unsigned int seqNr;
    const char* payload;
    unsigned int payloadSize;
    int ttl;

friend struct PacketPredicate;
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
        return packet1->equals(packet2);
    }
};

ARA_NAMESPACE_END

#endif // PACKET_H_
