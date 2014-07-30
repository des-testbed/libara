/*
 * $FU-Copyright$
 */

#ifndef TESTBED_PACKET_H_
#define TESTBED_PACKET_H_

#include "Testbed.h"
#include "Address.h"
#include "Exception.h"
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
        /**
         * @see Packet::Packet(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0);
         */
        TestbedPacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0);

        virtual ~TestbedPacket(){};

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

        dessert_msg_t* getMessage() const;

        void setMessage(dessert_msg_t* message);

        void setPayloadLength(unsigned int newPayloadLength);

        /**
         * The method returs a libdessert representation of a 
         * testbed packet (meaning a dessert_msg_t*)
         *
         * @return On success the method returns a dessert_msg_t*
         * structure of a TestbedPacket
         */
        dessert_msg_t* toDessertMessage() const;

    private:
        mutable std::mutex mutex;

	    mutable std::mutex senderMutex;
	    mutable std::mutex sourceMutex;
	    mutable std::mutex destinationMutex;

        dessert_msg_t* message = nullptr;

        /**
         * The payload of the packet.
         */
        void* payload = nullptr;
};

TESTBED_NAMESPACE_END

#endif 
