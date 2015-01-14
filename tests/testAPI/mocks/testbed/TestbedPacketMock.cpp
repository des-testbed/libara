/*
 * $FU-Copyright$
 */

#include "TestbedPacketMock.h"

TESTBED_NAMESPACE_BEGIN

TestbedPacketMock::TestbedPacketMock(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) : TestbedPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize) { 

}

TESTBED_NAMESPACE_END
