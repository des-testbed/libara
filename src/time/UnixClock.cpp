/*
 * $FU-Copyright$
 */

#include "UnixClock.h"
#include "UnixTime.h"
#include "UnixTimer.h"

using namespace ARA;

Time* UnixClock::makeTime() {
   return new UnixTime();
}

Timer* UnixClock::getNewTimer(char timerType) {
    return new UnixTimer(timerType);
}
