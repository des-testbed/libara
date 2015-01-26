/*
 * $FU-Copyright$
 */

#ifndef TESTBED_PACKET_H_
#define TESTBED_PACKET_H_

#include "Testbed.h"
#include "Address.h"
#include "Exception.h"
#include "Packet.h"

#include "spdlog/spdlog.h"

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

        /**
         * This method allows to set the payloadType member. The member holds the
         * ethernet type field of the DESSERT_EXT_ETH extension of libdessert.
         *
         * @param type The ether_type field of a DESSERT_EXT_ETH extension
         */
        void setPayloadType(u_short type);

        /**
         * This sets the payloadType member, which holds the
         * ethernet type field of the DESSERT_EXT_ETH
         */
        void addPayload(dessert_msg_t* message);

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

        // FIXME: get rid of the payload type since it doesn't change

        /**
         * The tun/tap interface expects a ethernet frame and not the raw
         * payload, so we have to safe the ethernet type of the ethernet
         * header (DESSERT_EXT_ETH).
         */
        u_short payloadType;

        std::shared_ptr<spdlog::logger> logger;
};

TESTBED_NAMESPACE_END

#endif 
