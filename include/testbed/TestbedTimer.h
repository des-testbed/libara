/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_TIMER_H_
#define _TESTBED_TIMER_H_

#include <mutex>
#include <chrono>
#include <thread>
#include <condition_variable>

#include "Timer.h"
#include "Testbed.h"
#include "ThreadInterruptedException.h"

TESTBED_NAMESPACE_BEGIN

class TestbedTimer : public Timer {
    public:
        TestbedTimer();
        virtual ~TestbedTimer();

        virtual void run(unsigned long timeoutInMicroSeconds);
        void sleep(unsigned long timeoutInMicroSeconds);
        virtual void interrupt();

    private:
        bool active;
        std::mutex mutex;
        std::thread timer;
};

TESTBED_NAMESPACE_END

#endif
