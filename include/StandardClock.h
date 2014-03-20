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
        Timer* getNewTimer(char timerType=-1, void* contextObject=nullptr);

        void scheduleTimer(std::function<void()> timer);

    private:
	ThreadPool threadPool;
};

ARA_NAMESPACE_END

#endif 
