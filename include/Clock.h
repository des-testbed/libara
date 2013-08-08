/*
 * $FU-Copyright$
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "Time.h"
#include "ARAMacros.h"
#include "Timer.h"

ARA_NAMESPACE_BEGIN

/**
 * A Clock is responsible for instantiating concrete instances of
 * the abstract Time and Timer class. For each concrete implementation
 * of Time and timer there will be one Clock counterpart that will
 * handle its instantiation.
 */
class Clock {
    public:
        virtual ~Clock() {};

        /**
         * Creates a new Time instance. The pointer must be deleted by the
         * invoking object.
         */
        virtual Time* makeTime() = 0;

        /**
         * Creates a new Timer instance. The pointer must be deleted by the
         * invoking object.
         *
         * @param timerType : parameter with which the timer can be assigned to some type.
         *                    This is helpful to distinguish conceptually different timers.
         * @param contextObject: an optional parameter which can provide some context when the timer is expired
         */
        virtual TimerPtr getNewTimer(char timerType=-1, void* contextObject=nullptr) = 0;
        //TODO use the TimerType enum and remove the default parameter
};

ARA_NAMESPACE_END

#endif
