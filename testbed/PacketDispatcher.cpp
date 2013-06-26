/*
 * $FU-Copyright$
 */

#include "PacketDispatcher.h"

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
    AddressPtr source;
    AddressPtr destination;
    AddressPtr sender;
    char type = 0x01;
    unsigned int sequenceNumber = dessertMessage->u16;
    int ttl = dessertMessage->ttl;

    return new Packet(source, destination, sender, type, sequenceNumber, ttl);
}

TESTBED_NAMESPACE_END
