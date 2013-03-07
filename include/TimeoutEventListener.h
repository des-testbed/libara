/*
 * $FU-Copyright$
 */

#ifndef TIMEOUT_EVENT_LISTENER_H_
#define TIMEOUT_EVENT_LISTENER_H_

namespace ARA {

class Timer;

    /**
     * The TimeoutEventListener is used by the Timer class. Its the listener interface
     * which is notified in case a timer timed out.
     */
    class TimeoutEventListener {
    public:
        virtual ~TimeoutEventListener() {}
        virtual void timerHasExpired(Timer* responsibleTimer) = 0;
    };
}

#endif 
