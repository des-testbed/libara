/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_TIMER_H_
#define _TESTBED_TIMER_H_

#include <chrono>
#include <thread>

#include "Timer.h"
#include "Testbed.h"
#include "ThreadInterruptedException.h"

TESTBED_NAMESPACE_BEGIN

class TestbedTimer : public Timer {
    public:
        TestbedTimer(char type, void* contextObject=nullptr);
        virtual ~TestbedTimer();

        virtual void run(unsigned long timeoutInMicroSeconds);
        void sleep(unsigned long timeoutInMicroSeconds);
        virtual void interrupt();

    private:
        bool active;
        bool timerIsRunning;
        std::thread* timer;
};

TESTBED_NAMESPACE_END

#endif
