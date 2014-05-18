/*
 * $FU-Copyright$
 */

#ifndef TESTBED_PACKET_H_
#define TESTBED_PACKET_H_

#include "Testbed.h"
#include "Address.h"
#include "Packet.h"

#include <mutex>

TESTBED_NAMESPACE_BEGIN

class TestbedPacket : public Packet {
    public:
        TestbedPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0);
        virtual ~TestbedPacket(){};

        AddressPtr getSource() const;
        AddressPtr getDestination() const;
        AddressPtr getSender() const;
        AddressPtr getPreviousHop() const;

    private:
        mutable std::mutex mutex;
};

TESTBED_NAMESPACE_END

#endif 
