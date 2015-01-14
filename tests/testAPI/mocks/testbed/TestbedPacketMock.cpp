/*
 * $FU-Copyright$
 */

#include "TestbedPacketMock.h"

TESTBED_NAMESPACE_BEGIN

TestbedPacketMock::TestbedPacketMock(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload, unsigned int payloadSize) : TestbedPacket(source, destination, sender, type, seqNr, ttl, payload, payloadSize) { 

}

void TestbedPacketMock::setSender(std::shared_ptr<Address> newSenderAddress) {
    sender = newSenderAddress;
}

void TestbedPacketMock::setSource(std::shared_ptr<Address> newSourceAddress) {
    source = newSourceAddress;
}

TESTBED_NAMESPACE_END
