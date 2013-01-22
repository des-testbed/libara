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

#include "CppUTest/TestHarness.h"
#include "OMNeTPacket.h"
#include "OMNeTAddress.h"
#include "PacketType.h"
#include "testAPI/mocks/AddressMock.h"

using namespace ARA;

typedef std::shared_ptr<Address> AddressPtr;

TEST_GROUP(OMNeTPacketTest) {};

// FIXME make this test work (segfault)
IGNORE_TEST(OMNeTPacketTest, testCreate) {
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    char type=PacketType::DATA;
    unsigned int originalSeqenceNumber=123;
    const char* payload="Hello World";
    unsigned int payloadSize=11;
    unsigned int originalHopCount = 3;

    OMNeTPacket packet2 = OMNeTPacket(originalSource, originalDestination, originalSender, type, originalSeqenceNumber, payload, payloadSize, originalHopCount);
}

// FIXME make this test work (segfault)
IGNORE_TEST(OMNeTPacketTest, testCreateDuplicateWarning) {
    // prepare the test
    AddressPtr originalSource (new AddressMock("source"));
    AddressPtr originalDestination (new AddressMock("destination"));
    AddressPtr originalSender (new AddressMock("sender"));
    char type=PacketType::DATA;
    unsigned int originalSeqenceNumber=123;
    const char* payload="Hello World";
    unsigned int payloadSize=11;
    unsigned int originalHopCount = 3;

    // create duplicateWarning packet
    OMNeTPacket packet = OMNeTPacket(originalSource, originalDestination, originalSender, type, originalSeqenceNumber, payload, payloadSize, originalHopCount);
    Packet* duplicateWarning = packet.createDuplicateWarning();

    // check packet contents
    CHECK(duplicateWarning->getSource()->equals(originalSource));
    CHECK(duplicateWarning->getDestination()->equals(originalDestination));
    // The sender of the packet will be determined when it is actually send by the ARA client
    CHECK_EQUAL(PacketType::DUPLICATE_ERROR, duplicateWarning->getType());
    CHECK_EQUAL(originalSeqenceNumber, duplicateWarning->getSequenceNumber());
    CHECK_EQUAL(0, duplicateWarning->getPayloadLength());
    CHECK_EQUAL(originalHopCount+1, duplicateWarning->getHopCount());

    // check if packet is a OMNeTPacket instance (important for the multiple inheritance stuff
    CHECK(dynamic_cast<OMNeTPacket*>(duplicateWarning) != NULL);

    delete duplicateWarning;
}
