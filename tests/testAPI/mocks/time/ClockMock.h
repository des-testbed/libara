/*
 * $FU-Copyright$
 */

#ifndef CLOCK_MOCK_H_
#define CLOCK_MOCK_H_

#include "Clock.h"
#include "TimeMock.h"
#include "TimerMock.h"

ARA_NAMESPACE_BEGIN

typedef std::shared_ptr<TimerMock> TimerMockPtr;

class ClockMock : public Clock {
    public:
        Time* makeTime();
        TimerPtr getNewTimer(char timerType=-1, void* contextObject=nullptr);

        TimerMockPtr getLastTimer();

    private:
        TimerMockPtr lastTimer = nullptr;
};

ARA_NAMESPACE_END

#endif
