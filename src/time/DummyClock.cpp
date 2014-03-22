/*
 * $FU-Copyright$
 */

#include "DummyClock.h"
#include "StandardTime.h"
#include "StandardTimer.h"

ARA_NAMESPACE_BEGIN

Time* DummyClock::makeTime(){
    return new StandardTime();
}

TimerPtr DummyClock::getNewTimer(TimerType timerType, void* contextObject){
    return std::make_shared<StandardTimer>(timerType, contextObject);
}

ARA_NAMESPACE_END
