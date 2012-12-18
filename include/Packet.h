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

namespace ARA {

/**
 * Packets encapsulate a payload that has to be transmitted from
 * a source node to a destination node.
 */
class Packet {
public:
    Packet(Address* source, Address* destination, Address* sender, char type, unsigned int seqNr, const char* payload=NULL, unsigned int payloadSize=0, unsigned int hopCount = 0);
    ~Packet();

    /**
     * Returns the address of the node that has initially created and send a packet
     * with this payload and sequence number directed at the destination node.
     * It must not be confused with the sender of this packet which corresponds
     * to the node from which this packet has been received (layer 2).
     *
     * @see Packet::getDestination()
     * @see Packet::getSender()
     */
    Address* getSource();

    /**
     * Returns the address of the node to whom the payload of this packet is directed.
     * It will not necessarily be the layer 2 receiver of this packet if the packet is
     * relayed through the network.
     *
     * @see Packet::getSource()
     * @see Packet::getSender()
     */
    Address* getDestination();

    /**
     * Returns the address of the node from which this packet has been received (layer 2).
     * This is not necessarily the original source of the packets information which is
     * returned by Packet::getSource().
     *
     * @see Packet::getSource()
     * @see Packet::getDestination()
     */
    Address* getSender();

    /**
     * Returns the type of this packet as an integer. The integer mapping is defined in
     * the PacketType enum class.
     */
    char getType();

    /**
     * Returns the sequence number of this packet. It has been set once by the packets
     * source and will not be modified on the packets path through the network.
     */
    unsigned int getSequenceNumber();

    /**
     * Returns the number of links this packet has been send over or respectively
     * the number of nodes this packet has been send by to arrive at the current node.
     * Note: After a packet has been received from another node this will be at least 1.
     */
    unsigned int getHopCount();
    const char* getPayload();
    unsigned int getPayloadLength();

    /**
     * Two packets are defined to be equal if they have the same source and sequence number
     * TODO is it really okay to define equality this way? What about the payload, destination and type?
     */
    bool equals(Packet* otherPacket);

    void setHopCount(unsigned int newValue);

    Packet* clone();
	Packet* createFANT(unsigned int sequenceNumber);

protected:
    Address* source;
    Address* destination;
    Address* sender;
    char type;
    unsigned int seqNr;
    const char* payload;
    unsigned int payloadSize;
    unsigned int hopCount;
};

} /* namespace ARA */
#endif // PACKET_H_
