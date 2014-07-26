/*
 * $FU-Copyright$
 */

#ifndef STANDARD_CLOCK_H_
#define STANDARD_CLOCK_H_

#include "ARAMacros.h"
#include "Clock.h"
#include "ThreadPool.h"
#include "StandardTimer.h"
#include "StandardTimerProxy.h"

#include <mutex>
#include <thread>
#include <vector>
#include <iostream>

ARA_NAMESPACE_BEGIN
class StandardTimer;

class StandardClock : public Clock {
    public:

        Time* makeTime();
        TimerPtr getNewTimer(TimerType timerType=TimerType::INVALID_TIMER, void* contextObject=nullptr);

        void scheduleTimer(unsigned long identifier, unsigned long timeoutInMicroseconds);
        void scheduleTimer(std::function<void()> timer);
        void interruptTimer(unsigned long identifier);

//        std::string listActiveTimers();

    private:
        unsigned long identifier = 0;

	/**
	 * @brief The member manages the access to a thread pool implementation.
	 */
	ThreadPool threadPool;

	/**
	 *
	 */
	std::vector<std::shared_ptr<StandardTimer>> timerList;

	/**
	 * The mutex protects the access to critical sections, such as the access to members
	 * of this class.
	 */
	std::mutex mutex;
};

ARA_NAMESPACE_END

#endif 
