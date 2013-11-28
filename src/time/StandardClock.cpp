/*
 * $FU-Copyright$
 */

#include "StandardClock.h"
#include "StandardTime.h"
#include "StandardTimer.h"

ARA_NAMESPACE_BEGIN

Time* StandardClock::makeTime(){
    return new StandardTime();
}

Timer* StandardClock::getNewTimer(char timerType, void* contextObject){
    return new StandardTimer(timerType, contextObject);
}

ARA_NAMESPACE_END
