/*
 * $FU-Copyright$
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "Time.h"
#include "Timer.h"

namespace ARA {

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
             * @param timerType : an optional parameter with which the timer can be assigned to some type.
             *                    This is helpful to distinguish conceptually different timers.
             */
            virtual Timer* getNewTimer(char timerType=0) = 0;
    };
}

#endif
