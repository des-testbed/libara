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

void StandardClock::runTimer(std::thread::id id, unsigned long timeout){

}

void StandardClock::interruptTimer(std::thread::id id){
     //this->threadPool->interrupt(id);
}

ARA_NAMESPACE_END
