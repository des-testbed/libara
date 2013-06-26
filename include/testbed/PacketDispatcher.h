/*
 * $FU-Copyright$
 */

#ifndef PACKET_DISPATCHER_H
#define PACKET_DISPATCHER_H

#include "CLibs.h"
#include "Testbed.h"
#include "Packet.h"

TESTBED_NAMESPACE_BEGIN

_dessert_cb_results PacketToMeshDispatcher (dessert_msg_t* ReceivedMessage, uint32_t Length, dessert_msg_proc_t *ProcessingFlags, dessert_sysif_t *SystemInterface, dessert_frameid_t id);
_dessert_cb_results PacketToSystemDispatcher (dessert_msg_t* ReceivedMessage, uint32_t Length, dessert_msg_proc_t *ProcessingFlags, dessert_meshif_t *MeshInterface, dessert_frameid_t id);
Packet* extractPacket(dessert_msg_t* dessertMessage);

TESTBED_NAMESPACE_END

#endif /*PACKET_DISPATCHER_H*/
