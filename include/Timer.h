	/*
 * $FU-Copyright$
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "TimeoutEventListener.h"
#include "ARAMacros.h"
#include "TimerType.h"

#include "StackTrace.h"

#include <deque>
#include <iostream>

ARA_NAMESPACE_BEGIN
class TimeoutEventListener;

class Timer : public std::enable_shared_from_this<Timer> {
    public:

        /**
         * Creates a new Timer instance.
         * The Timer can have a type and also an optional context object.
         * Both can be used by the TimeoutEventlisteners when the timer has expired.
         */
        Timer(char type, void* contextObject=nullptr);

        virtual ~Timer() {
           printStacktrace();
           std::cout << "[Timer] destructor called " << std::endl;
        }

        virtual void addTimeoutListener(TimeoutEventListener* listener);

        bool operator==(const Timer& otherTimer) const {
            return this->equals(&otherTimer);
        }

        bool operator==(const std::shared_ptr<Timer> otherTimer) const {
            return this->equals(otherTimer);
        }

        virtual bool equals(const Timer* otherTimer) const = 0;
        virtual bool equals(const std::shared_ptr<Timer> otherTimer) const = 0; // FIXME 2 abstract equals definitions is not necessary
        virtual size_t getHashValue() const = 0;

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

        /**
         * Assigns a new context object. The old one (if any) will *not* be deleted by the timer, so the caller
         * needs to take care of that.
         */
        void setContextObject(void* contextObject);

	/**
	 * The method returns a shared_ptr of 'this'. However, prior to calling shared_from_this 
	 * on an object t, there must be a std::shared_ptr that owns t, otherwise a bad_weak_ptr 
	 * exception is thrown.
	 */
	std::shared_ptr<Timer> getTimer() { return shared_from_this(); }

    protected:
        void notifyAllListeners();

    protected:
        char type;
        void* contextObject;

    private:
        std::deque<TimeoutEventListener*> listeners;
};

typedef std::shared_ptr<Timer> TimerPtr;

/**
 * This functor is needed for std::unordered_set
 */
struct TimerHash {
    size_t operator()(std::shared_ptr<Timer> timer) const {
        return timer->getHashValue();
    }
};

/**
 * This functor is needed for std::unordered_set 
 */
struct TimerPredicate {
    size_t operator()(std::shared_ptr<Timer> timer, std::shared_ptr<Timer> anotherTimer) const {
        return timer->equals(anotherTimer);
    }
};

ARA_NAMESPACE_END

#endif 
