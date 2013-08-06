/*
 * $FU-Copyright$
 */

#ifndef _STANDARD_TIMER_H_
#define _STANDARD_TIMER_H_

#include "ARAMacros.h"
#include "Timer.h"

#include <chrono>
#include <thread>

ARA_NAMESPACE_BEGIN

class StandardTimer : public Timer {
    public:
        StandardTimer(char type, void* contextObject=nullptr);
        virtual ~StandardTimer();

        virtual void run(unsigned long timeoutInMicroSeconds);
        void sleep(unsigned long timeoutInMicroSeconds);
        virtual void interrupt();

    private:
        void makeSureTimeIsNotRunning();
        void forcefullyStopTimer();

    private:
        bool timerHasBeenInterrupted;
        bool timerIsRunning;
        std::thread* timer;
};

ARA_NAMESPACE_END

#endif
