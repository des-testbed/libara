/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_PACKET_H_
#define _TESTBED_PACKET_H_

#include "Packet.h"
#include "Testbed.h"
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

class TestbedPacket : public Packet {
    public:
        TestbedPacket();
        TestbedPacket(dessert_msg_t* packet);
//        dessert_msg_t* createDessertMessage();
};

TESTBED_NAMESPACE_END

#endif // _TESTBED_PACKET_H_
