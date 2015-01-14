/*
 * $FU-Copyright$
 */

#ifndef TESTBED_NETWORK_INTERFACE_MOCK_H_
#define TESTBED_NETWORK_INTERFACE_MOCK_H_

#include "CLibs.h"
#include "Testbed.h"
#include "TestbedPacket.h"


TESTBED_NAMESPACE_BEGIN

class TestbedPacketMock : public TestbedPacket {
    public:
        TestbedPacketMock(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0);

};

TESTBED_NAMESPACE_END

#endif
