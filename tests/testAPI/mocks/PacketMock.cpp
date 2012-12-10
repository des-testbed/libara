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

#include "PacketMock.h"
#include "PacketType.h"

namespace ARA {

PacketMock::PacketMock() {
    this->source = new AddressMock("Source");
    this->destination = new AddressMock("Destination");
    this->type = PacketType::FANT;
    this->seqNr = 123;
    this->payload = "Hello World";
    this->payloadSize = sizeof("Hello World");
    this->hopCount = 3;
}

PacketMock::~PacketMock() {
    delete this->source;
    delete this->destination;
}

Address* PacketMock::getSource() {
    return source;
}

Address* PacketMock::getDestination() {
    return destination;
}

unsigned int PacketMock::getType() {
    return type;
}

unsigned int PacketMock::getSequenceNumber() {
    return seqNr;
}

unsigned int PacketMock::getHopCount() {
    return hopCount;
}

const char* PacketMock::getPayload() {
    return payload;
}

unsigned int PacketMock::getPayloadLength() {
    return payloadSize;
}

} /* namespace ARA */
