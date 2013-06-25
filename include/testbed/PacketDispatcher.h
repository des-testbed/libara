
#ifndef CLIBS_H
#include "CLibs.h"

#endif /* CLIBS_H */

#ifndef PACKET_DISPATCHER_H
#define PACKET_DISPATCHER_H

_dessert_cb_results PacketToMeshDispatcher (dessert_msg_t* ReceivedMessage, uint32_t Length, dessert_msg_proc_t *ProcessingFlags, dessert_sysif_t *SystemInterface, dessert_frameid_t id);
_dessert_cb_results PacketToSystemDispatcher (dessert_msg_t* ReceivedMessage, uint32_t Length, dessert_msg_proc_t *ProcessingFlags, dessert_meshif_t *MeshInterface, dessert_frameid_t id);

#endif /*PACKET_DISPATCHER_H*/
