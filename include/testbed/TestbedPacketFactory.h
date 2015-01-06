/*
 * $FU-Copyright$
 */

#ifndef TESTBED_PACKET_FACTORY_H_
#define TESTBED_PACKET_FACTORY_H_

#include "CLibs.h"
#include "Testbed.h"
#include "Exception.h"
#include "PacketFactory.h"
#include "TestbedPacket.h"
#include "TestbedARAClient.h"

TESTBED_NAMESPACE_BEGIN

struct RoutingExtension {
    /// ARA destination address
    u_int8_t  ara_dhost[ETH_ALEN];  
    /// ARA source address
    u_int8_t  ara_shost[ETH_ALEN];  
    /// reserved for future use
    u_int16_t ara_data;           
} __attribute__ ((__packed__));

typedef struct RoutingExtension RoutingExtension;

/**
 * The class provides a packet factory for testbed packets.
 *
 * @see PacketFactory
 */
class TestbedPacketFactory : public PacketFactory {
    public:
        /**
         * @see PacketFactory::PacketFactory(int maxHopCount)
         */
        TestbedPacketFactory(int maxHopCount);

        virtual ~TestbedPacketFactory() {};

        /**
         * @see PacketFactory::makeClone(const Packet* packet)
         */
        TestbedPacket* makeClone(const Packet* packet);

        //TODO: refactor
        virtual TestbedPacket* makeNewPacket(dessert_msg_t* message);

        TestbedPacket* makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, dessert_msg_t* message, unsigned int payloadSize);

        /**
         * @see PacketFactory::makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, const char* payload, unsigned int payloadSize);
         */
        TestbedPacket* makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, const char* payload, unsigned int payloadSize);

        /**
         * The method creates a dessert message (dessert_msg_t*) from a given
         * packet. This method is typically invoked if packets are transmitted
         * over the wire/air.
         *
         * @param packet The packet which should be converted to a dessert message
         * @param interface The interface the packet should be sent over (if
         * applicable)
         * @param nextHop The next hop of the packet (if applicable)
         *
         * @return On success the method returns the dessert_msg_t
         * representation of a packet.
         */
        dessert_msg_t* makeDessertMessage(const Packet* packet, dessert_meshif_t* interface, AddressPtr nextHop);

        /**
         * @see PacketFactory::makeFANT(AddressPtr source, AddressPtr destination, unsigned int
         * sequenceNumber)
         */
        TestbedPacket* makeFANT(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber);

        /**
         * @see PacketFactory::makeBANT(const Packet* packet, unsigned int
         * sequenceNumber)
         */
        TestbedPacket* makeBANT(const Packet *packet, unsigned int sequenceNumber);

        /**
         * @see PacketFactory::makeAcknowledgmentPacket(const Packet* originalPacket, AddressPtr sender)
         */
        TestbedPacket* makeAcknowledgmentPacket(const Packet* originalPacket, AddressPtr sender);

        /**
         * The method checks if a dessert_msg_t is consistent. It wraps the
         * functionality of dessert_msg_check() of libdessert.
         *
         * @param message The message which should be checked
         *
         * @return The method returns true if the message is consistent,
         * otherwise false.
         */
        bool checkDessertMessage(dessert_msg_t* message);

    protected:
        /**
         *
         */
        virtual TestbedPacket* makePacket(dessert_msg_t* message);

        /**
         * @see PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr)
         */
        virtual TestbedPacket* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr);

    private:

        RoutingExtension* getRoutingExtension(dessert_msg_t* message);



};

TESTBED_NAMESPACE_END

#endif 
