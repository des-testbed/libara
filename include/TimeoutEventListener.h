/*
 * $FU-Copyright$
 */

#ifndef TIMEOUT_EVENT_LISTENER_H_
#define TIMEOUT_EVENT_LISTENER_H_

#include "ARAMacros.h"

ARA_NAMESPACE_BEGIN

class Timer;

/**
 * The TimeoutEventListener is used by the Timer class. Its the listener interface
 * which is notified in case a timer timed out.
 */
class TimeoutEventListener {
public:
    virtual ~TimeoutEventListener() {}

    /**
     * Notifies this listener, that a timer has expired.
     * To distinguish several timers from one another, the listener can use
     * Timer::getType(). Additionally an optional contextObject can be given by the timer.
     */
    virtual void timerHasExpired(TimerPtr responsibleTimer) = 0;
};

ARA_NAMESPACE_END

#endif
