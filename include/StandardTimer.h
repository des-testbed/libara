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
        StandardTimer(TimerType type, void* contextObject=nullptr);
        virtual ~StandardTimer();

        virtual void run(unsigned long timeoutInMicroSeconds);
        void sleep(unsigned long timeoutInMicroSeconds);
        virtual void interrupt();
     
        bool equals(const Timer* otherTimer) const;
        bool equals(const std::shared_ptr<Timer> otherTimer) const;

        size_t getHashValue() const;

    private:
        std::condition_variable conditionVariable;
        std::mutex conditionVariableMutex;
        std::shared_ptr<std::thread> timer;
};

ARA_NAMESPACE_END

#endif
