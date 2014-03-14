/*
 * $FU-Copyright$
 */

#ifndef STANDARD_CLOCK_H_
#define STANDARD_CLOCK_H_

#include "ARAMacros.h"
//#include "ThreadPool.h"
#include "Clock.h"

#include <thread>
#include <vector>

ARA_NAMESPACE_BEGIN

class StandardClock : public Clock {
    public:
        Time* makeTime();
        Timer* getNewTimer(char timerType=-1, void* contextObject=nullptr);

	void runTimer(std::thread::id id, unsigned long timeout);
	void interruptTimer(std::thread::id id);
	void setTimeoutListener(std::thread::id id, std::vector<TimeoutEventListener*> listener);

    private:
//	ThreadPool threadPool;
};

ARA_NAMESPACE_END

#endif 
