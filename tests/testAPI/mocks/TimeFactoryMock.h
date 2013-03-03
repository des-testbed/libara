/*
 * $FU-Copyright$
 */

#ifndef TIMEFACTORY_MOCK_H_
#define TIMEFACTORY_MOCK_H_

#include "TimeFactory.h"
#include "TimeMock.h"

namespace ARA {
    class TimeFactoryMock : public TimeFactory {
        public:
            Time* makeTime();
    };
}

#endif
