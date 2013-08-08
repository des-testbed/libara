/*
 * $FU-Copyright$
 */

#include "UnixClock.h"
#include "UnixTime.h"
#include "UnixTimer.h"

ARA_NAMESPACE_BEGIN

Time* UnixClock::makeTime() {
   return new UnixTime();
}

TimerPtr UnixClock::getNewTimer(char timerType, void* contextObject) {
    return TimerPtr(new UnixTimer(timerType, contextObject));
}

ARA_NAMESPACE_END
