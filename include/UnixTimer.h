/*
 * $FU-Copyright$
 */

#ifndef UNIXTIMER_H_
#define UNIXTIMER_H_

#include "Timer.h"

namespace ARA {

    /**
     * FIXME implement this class
     */
    class UnixTimer : public Timer {
    public:
        void run(long timeInMilliSeconds);
        void interrupt();
    };
}

#endif 

