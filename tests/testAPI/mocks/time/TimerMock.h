/*
 * $FU-Copyright$
 */

#ifndef TIMER_MOCK_H_
#define TIMER_MOCK_H_

#include "Timer.h"

namespace ARA {
    class TimerMock : public Timer {
    public:
        void run(unsigned long timeoutInMicroSeconds);
        void interrupt();

        void expire();
        bool hasExpired() const;
        bool isRunning() const;
        bool hasBeenInterrupted() const;

    private:
        bool isTimerRunning = false;
        bool hasTimerExpired = false;
        bool hasTimerBeenInterrupted = false;
    };
}

#endif
