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
             */
            virtual Timer* getNewTimer() = 0;
    };
}

#endif
