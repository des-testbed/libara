/*
 * $FU-Copyright$
 */

#ifndef CLOCK_MOCK_H_
#define CLOCK_MOCK_H_

#include "Clock.h"
#include "TimeMock.h"
#include "TimerMock.h"
#include "TimerType.h"

namespace ARA {
    class ClockMock : public Clock {
        public:
            Time* makeTime();
            Timer* getNewTimer(TimerType timerType=TimerType::INVALID_TIMER, void* contextObject=nullptr);

            TimerMock* getLastTimer();

        private:
            TimerMock* lastTimer = nullptr;
            //std::weak_ptr<TimerMock> lastTimer = nullptr;
    };
}

#endif
