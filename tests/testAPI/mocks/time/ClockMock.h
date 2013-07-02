/*
 * $FU-Copyright$
 */

#ifndef CLOCK_MOCK_H_
#define CLOCK_MOCK_H_

#include "Clock.h"
#include "TimeMock.h"
#include "TimerMock.h"

namespace ARA {
    class ClockMock : public Clock {
        public:
            Time* makeTime();
            Timer* getNewTimer(char timerType=-1, void* contextObject=nullptr);

            TimerMock* getLastTimer();

        private:
            TimerMock* lastTimer = nullptr;
    };
}

#endif
