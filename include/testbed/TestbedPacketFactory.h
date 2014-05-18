/*
 * $FU-Copyright$
 */

#ifndef TESTBED_PACKET_FACTORY_H_
#define TESTBED_PACKET_FACTORY_H_

#include "Testbed.h"
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

        //TODO: refactor
        virtual TestbedPacket* makeNewPacket(dessert_msg_t* message);

        TestbedPacket* makeDataPacket(dessert_msg_t* message);
        TestbedPacket* makeDataPacket(AddressPtr source, AddressPtr destination, unsigned int sequenceNumber, const char* payload, unsigned int payloadSize);

        dessert_msg_t* makeDessertMessage(const Packet* packet, dessert_meshif_t* interface, AddressPtr nextHop);

    protected:
        virtual TestbedPacket* makePacket(dessert_msg_t* message);
        virtual TestbedPacket* makePacket(AddressPtr source, AddressPtr destination, AddressPtr sender, char type, unsigned int seqNr, int ttl, const char* payload=nullptr, unsigned int payloadSize=0, AddressPtr previousHop=nullptr);
        
    private:
        /**
         *
         */
        ether_header* getEthernetHeader(dessert_msg_t* message);
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
};

TESTBED_NAMESPACE_END

#endif 
