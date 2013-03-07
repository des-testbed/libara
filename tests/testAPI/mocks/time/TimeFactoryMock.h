/*
 * $FU-Copyright$
 */

#ifndef TIMEFACTORY_MOCK_H_
#define TIMEFACTORY_MOCK_H_

#include "TimeFactory.h"
#include "TimeMock.h"
#include "TimerMock.h"

namespace ARA {
    class TimeFactoryMock : public TimeFactory {
        public:
            Time* makeTime();
            Timer* getNewTimer();

            TimerMock* getLastTimer();

        private:
            TimerMock* lastTimer = nullptr;
    };
}

#endif
