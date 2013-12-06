/*
 * $FU-Copyright$
 */

#ifndef _STANDARD_TIMER_H_
#define _STANDARD_TIMER_H_

#include "ARAMacros.h"
#include "Timer.h"
#include "StandardTimerThread.h"

#include <chrono>
#include <thread>
#include <condition_variable>

ARA_NAMESPACE_BEGIN

class StandardTimer : public Timer {
    public:
        StandardTimer(char type, void* contextObject=nullptr);
        virtual ~StandardTimer();

        virtual void run(unsigned long timeoutInMicroSeconds);
        void sleep(unsigned long timeoutInMicroSeconds);
        virtual void interrupt();

    private:
        std::condition_variable conditionVariable;
        std::mutex conditionVariableMutex;
};

ARA_NAMESPACE_END

#endif
