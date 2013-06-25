/*
 * $FU-Copyright$
 */

#ifndef _MESH_HANDLER_H_
#define _MESH_HANDLER_H_

#include "CLibs.h"

class MeshHandler {
    public:
        virtual dessert_cb_result handler(dessert_msg_t* msg, size_t len, dessert_msg_proc_t* proc, dessert_meshif_t* iface_in, dessert_frameid_t id) = 0;
};

#endif 
