/*
 * $FU-Copyright$
 */

#ifndef TIMER_MOCK_H_
#define TIMER_MOCK_H_

#include "ARAMacros.h"
#include "Timer.h"

ARA_NAMESPACE_BEGIN

class TimerMock : public Timer {
    public:
        TimerMock(TimerType type=TimerType::INVALID_TIMER, void* contextObject=nullptr) : Timer(type, contextObject) {};
        void run(unsigned long timeoutInMicroSeconds);
        void interrupt();

        void expire();
        bool hasExpired() const;
        bool isRunning() const;
        bool hasBeenInterrupted() const;

        bool equals(const Timer* otherTimer) const;
        bool equals(const std::shared_ptr<Timer> otherTimer) const;

        size_t getHashValue() const;

    private:
        bool isTimerRunning = false;
        bool hasTimerExpired = false;
        bool hasTimerBeenInterrupted = false;
};

ARA_NAMESPACE_END

#endif
