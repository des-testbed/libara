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

/**
 * An implementation of class Packet for the Testbed component of the libARA
 * framework
 *
 * @see Packet
 */
class TestbedPacket : public Packet {
    public:
        TestbedPacket(){};
        TestbedPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0);
        virtual ~TestbedPacket(){};

        /**
         * The logical copy constructor makes a true copy of the TestbedPacket as
         * well as its dynamic structures. This means that a copy holds new
         * shared pointers to the addresses and a copy of the payload.
         */
        TestbedPacket(const TestbedPacket& packet);

        /**
         * @see Packet::getSource()
         */
        AddressPtr getSource() const;

        /**
         * @see Packet::getDestination()
         */
        AddressPtr getDestination() const;

        /**
         * @see Packet::getSender()
         */
        AddressPtr getSender() const;

        /**
         * @see Packet::getPreviousHop()
         */
        AddressPtr getPreviousHop() const;

    private:
        mutable std::mutex mutex;
};

TESTBED_NAMESPACE_END

#endif 
