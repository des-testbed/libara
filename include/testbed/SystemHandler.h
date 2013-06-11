#ifndef _SYSTEM_HANDLER_H_
#define _SYSTEM_HANDLER_H_

#include "CLibs.h"

class SystemHandler {
    public:
        virtual dessert_cb_result handler(dessert_msg_t* msg, size_t len, dessert_msg_proc_t* proc, dessert_sysif_t* tunif, dessert_frameid_t id) = 0;
};

#endif 
