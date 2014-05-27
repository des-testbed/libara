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

        TestbedPacket* makeDataPacket(dessert_msg_t* message);
        TestbedPacket* makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, struct ether_header* payload, unsigned int payloadSize);
        TestbedPacket* makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, 
            const char* payload, unsigned int payloadSize);

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

        virtual TestbedPacket* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, struct ether_header* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr);
        
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

        /**
         *
         */
        struct ether_header* getEthernetHeader(dessert_msg_t* message);
        /**
         *
         */
        void setEthernetHeader(dessert_msg_t* message, dessert_meshif_t* interface, AddressPtr nextHop);

        /**
         *
         */
        RoutingExtension* getRoutingExtension(dessert_msg_t* message);
        /**
         *
         */
        void setRoutingExtension(dessert_msg_t* message, u_int8_t* source, u_int8_t* destination);

        /**
         * The method extracts an ethernet frame from a dessert message. The
         * memory allocation of the ethernet frame happens in the method. 
         *
         * @param message the dessert message where the payload should be
         * extracted
         * @param payload the resulting ethernet frame
         *
         * @return the size of the freshly allocated ethernet frame 
         */
        int getPayload(dessert_msg_t* message, struct ether_header **payload);
};

TESTBED_NAMESPACE_END

#endif 
