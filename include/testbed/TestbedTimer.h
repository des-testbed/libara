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
        virtual void interrupt();

        class Runner {
            public:
                void run(unsigned long timeoutInMicroSeconds) {
                    try {
                        /// set the sleep time
                        std::chrono::microseconds duration(timeoutInMicroSeconds);
                        /// set thread to sleep
                        std::this_thread::sleep_for(duration);
                    } catch (ThreadInterruptedException&) {
                        /// do something smart
		    
                    }
                }

                void interrupt() {
                    throw ThreadInterruptedException();
                }
        };
};

TESTBED_NAMESPACE_END

#endif
