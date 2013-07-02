/*
 * $FU-Copyright$
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "ARAMacros.h"
#include "TimeoutEventListener.h"

#include <deque>

ARA_NAMESPACE_BEGIN

class Timer {
    public:

        /**
         * Creates a new Timer instance.
         * The Timer can have a type and also an optional context object.
         * Both can be used by the TimeoutEventlisteners when the timer has expired.
         */
        Timer(char type, void* contextObject=nullptr);
        virtual ~Timer() {}

        void addTimeoutListener(TimeoutEventListener* listener);

        /**
         * This method is used to start the timer. The timer is required to run
         * the stated amount of microseconds.
         */
        virtual void run(unsigned long timeoutInMicroSeconds) = 0;

        /**
         * Interrupts a running timer.
         * This should stop the timer and no TimeoutEventListener should be notified.
         * Interrupting a timer which has not been started should do nothing.
         */
        virtual void interrupt() = 0;

        /**
         * Returns the type of this timer. This is useful for objects to distinguish
         * several conceptually different timers.
         */
        char getType() const;

        /**
         * Returns the context object.
         * If there is no context object, this will always return a nullptr.
         */
        void* getContextObject();

    protected:
        void notifyAllListeners();

    protected:
        char type;
        void* contextObject;

    private:
        std::deque<TimeoutEventListener*> listeners;
};

ARA_NAMESPACE_END

#endif 
