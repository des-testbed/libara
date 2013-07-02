/*
 * $FU-Copyright$
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "ARAMacros.h"
#include "Time.h"
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
         */
        virtual Timer* getNewTimer(char timerType=-1) = 0;
        //TODO use the TimerType enum and remove the default parameter
};

ARA_NAMESPACE_END

#endif
