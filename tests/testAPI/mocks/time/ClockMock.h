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
            TimerPtr getNewTimer(char timerType,  void* contextObject=nullptr);

            std::weak_ptr<TimerMock> getLastTimer();

        private:
            std::weak_ptr<TimerMock> lastTimer;
    };
}

#endif
