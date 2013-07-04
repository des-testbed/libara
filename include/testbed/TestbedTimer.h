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
        virtual ~TestbedTimer();

        virtual void run(unsigned long timeoutInMicroSeconds);
        void sleep(unsigned long timeoutInMicroSeconds);
        virtual void interrupt();
};

TESTBED_NAMESPACE_END

#endif
