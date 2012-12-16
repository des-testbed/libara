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
    Packet(Address* source, Address* destination, char type, unsigned int seqNr, const char* payload=NULL, unsigned int payloadSize=0, unsigned int hopCount = 0);
    ~Packet();

    Address* getSource();
    Address* getDestination();
    char getType();
    unsigned int getSequenceNumber();
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
    char type;
    unsigned int seqNr;
    const char* payload;
    unsigned int payloadSize;
    unsigned int hopCount;
};

} /* namespace ARA */
#endif // PACKET_H_
