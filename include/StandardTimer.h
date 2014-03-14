/*
 * $FU-Copyright$
 */

#ifndef _STANDARD_TIMER_H_
#define _STANDARD_TIMER_H_

#include "ARAMacros.h"
#include "Timer.h"
#include "Environment.h"
#include "StandardClock.h"

#include <thread>

ARA_NAMESPACE_BEGIN

class StandardTimer : public Timer {
    public:
        StandardTimer(char type, void* contextObject=nullptr);
        virtual ~StandardTimer();

        virtual void run(unsigned long timeoutInMicroSeconds);
        virtual void interrupt();

	void setThreadIdentifier(std::thread::id identifier);

    private:
	std::thread::id timerIdentifier;
};

ARA_NAMESPACE_END

#endif
