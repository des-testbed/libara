/*
 * $FU-Copyright$
 */

#ifndef TESTBED_PACKET_FACTORY_H_
#define TESTBED_PACKET_FACTORY_H_

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
        TestbedPacketFactory(int maxHopCount);
        virtual ~TestbedPacketFactory() {};

        /**
         * @see PacketFactory::makeClone(const Packet* packet)
         */
        Packet* makeClone(const Packet* packet);

        //TODO: refactor
        virtual TestbedPacket* makeNewPacket(dessert_msg_t* message);

        TestbedPacket* makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, dessert_msg_t* message, unsigned int payloadSize);
        TestbedPacket* makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, 
            const char* payload, unsigned int payloadSize);

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
         * @see PacketFactory::makeFANT(const Packet* packet)
         */
        dessert_msg_t* makeFANT(const Packet *packet);
        /**
         * @see PacketFactory::makeBANT(const Packet* packet)
         */
        dessert_msg_t* makeBANT(const Packet *packet);

    protected:
        virtual TestbedPacket* makePacket(dessert_msg_t* message);
        /**
         * @see PacketFactory::makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr)
         */
        virtual TestbedPacket* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr);

        virtual TestbedPacket* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, dessert_msg_t* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr);
        
    private:
        /**
         * The method creates a forward or backward agent depending of the type
         * of the packet.
         *
         * @args packet The (ant) packet which should be converted into a
         * dessert_msg_t type.
         *
         * @return on success the dessert_msg_t representation of the packet
         */
        dessert_msg_t* makeAntAgent(const Packet *packet);
};

TESTBED_NAMESPACE_END

#endif 
