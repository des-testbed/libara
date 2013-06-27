/*
 * $FU-Copyright$
 */

#include "PacketDispatcher.h"
#include "TestbedAddress.h"

TESTBED_NAMESPACE_BEGIN

_dessert_cb_results PacketToMeshDispatcher (dessert_msg_t* ReceivedMessage, uint32_t Length, dessert_msg_proc_t* ProcessingFlags, dessert_sysif_t* SystemInterface, dessert_frameid_t id) {
    dessert_debug("PacketToMeshDispatcher: Received packet from sys, broadcasting via mesh");

    if(dessert_meshsend(ReceivedMessage,NULL) == DESSERT_OK){
        return DESSERT_MSG_DROP;
    }

    return DESSERT_MSG_NEEDNOSPARSE;
}

_dessert_cb_results PacketToSystemDispatcher (dessert_msg_t* ReceivedMessage, uint32_t Length, dessert_msg_proc_t *ProcessingFlags, dessert_meshif_t *MeshInterface, dessert_frameid_t id) {
    dessert_debug("PacketToSystemDispatcher: Sending packet");

    struct ether_header *eth;
    size_t eth_len;

    if (ProcessingFlags->lflags & DESSERT_RX_FLAG_L25_DST ||
        ProcessingFlags->lflags & DESSERT_RX_FLAG_L25_BROADCAST ||
        ProcessingFlags->lflags & DESSERT_RX_FLAG_L25_MULTICAST ) {
            eth_len = dessert_msg_ethdecap(ReceivedMessage, &eth);
            dessert_syssend(eth, eth_len);
            free(eth);
    }

    return DESSERT_MSG_KEEP;
}

Packet* extractPacket(dessert_msg_t* dessertMessage) {
    ether_header* ethernetFrame = extractEthernetHeader(dessertMessage);

    AddressPtr source (new TestbedAddress(ethernetFrame->ether_shost));
    AddressPtr destination (new TestbedAddress(ethernetFrame->ether_dhost));
    AddressPtr sender; //TODO ?

    char packetType = dessertMessage->u8;
    unsigned int sequenceNumber = dessertMessage->u16;
    int ttl = dessertMessage->ttl;

    void* payload;
    unsigned int payloadSize =  dessert_msg_getpayload(dessertMessage, &payload);

    return new Packet(source, destination, sender, packetType, sequenceNumber, ttl, (const char*)payload, payloadSize);
}

ether_header* extractEthernetHeader(dessert_msg_t* dessertMessage) {
    dessert_ext_t* extension;
    dessert_msg_getext(dessertMessage, &extension, DESSERT_EXT_ETH, 0);
    return (ether_header*) extension->data;
}

TESTBED_NAMESPACE_END
