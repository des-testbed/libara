/*
 * $FU-Copyright$
 */

#include "StandardTimer.h"

ARA_NAMESPACE_BEGIN

StandardTimer::StandardTimer(char type, void* contextObject) : Timer(type, contextObject) { }

StandardTimer::~StandardTimer(){ }

void StandardTimer::run(unsigned long timeoutInMicroSeconds){
    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());
    clock->runTimer(timerIdentifier, timeoutInMicroSeconds);
}

void StandardTimer::interrupt(){
    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());
    clock->interruptTimer(timerIdentifier);
}

void StandardTimer::setThreadIdentifier(std::thread::id identifier){
    this->timerIdentifier = identifier;
}

ARA_NAMESPACE_END
