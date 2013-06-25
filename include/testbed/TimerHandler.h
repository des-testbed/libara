/*
 * $FU-Copyright$
 */

#ifndef _TIMER_HANDLER_H_
#define _TIMER_HANDLER_H_

#include "CLibs.h"

class TimerHandler {
    public:
        virtual dessert_per_result_t handler(void* data, struct timeval* scheduled, struct timeval* interval) = 0;
};

#endif 
