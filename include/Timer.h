/*
 * $FU-Copyright$
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "TimeoutEventListener.h"

#include <deque>

namespace ARA {
    class Timer {
    public:
        Timer();
        virtual ~Timer() {}
        void addTimeoutListener(TimeoutEventListener* listener);

        /**
         * This method is used to start the timer. The timer is required to run
         * the stated amount of milliseconds.
         */
        virtual void run(long timeInMilliSeconds) = 0;

        /**
         * Interrupts a running timer.
         * This should stop the timer and no TimeoutEventListener should be notified.
         * Interrupting a timer which has not been started should do nothing.
         */
        virtual void interrupt() = 0;

    protected:
        void notifyAllListeners();

    private:
        std::deque<TimeoutEventListener*> listeners;
    };
}

#endif 
