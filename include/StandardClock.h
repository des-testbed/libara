/*
 * $FU-Copyright$
 */

#ifndef STANDARD_CLOCK_H_
#define STANDARD_CLOCK_H_

#include "ARAMacros.h"
#include "ThreadPool.h"
#include "Clock.h"

#include <thread>
#include <vector>

ARA_NAMESPACE_BEGIN
class StandardClock : public Clock {
    public:
        Time* makeTime();
        TimerPtr getNewTimer(TimerType timerType=TimerType::INVALID_TIMER, void* contextObject=nullptr);

        void scheduleTimer(std::function<void()> timer);

    private:
	/**
	 * @brief The member manages the access to a thread pool implementation 
	 *        the timer tasks. 
	 */
	ThreadPool threadPool;
};

ARA_NAMESPACE_END

#endif 
