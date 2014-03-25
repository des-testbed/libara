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

    private:

	/**
	 * @brief The member manages the access to a thread pool implementation 
	 *        the timer tasks. 
	 */
	ThreadPool threadPool;

	std::vector<std::shared_ptr<StandardTimer> > timerList;
};

ARA_NAMESPACE_END

#endif 
