/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_TIMER_H_
#define _TESTBED_TIMER_H_

#include <chrono>

#include "Timer.h"
#include "Testbed.h"

TESTBED_NAMESPACE_BEGIN

class TestbedTimer : public Timer {
    public:
        TestbedTimer();
        virtual ~TestbedTimer();

        virtual void run(unsigned long timeoutInMicroSeconds);

        virtual void interrupt();
};

TESTBED_NAMESPACE_END

#endif
