/*
 * $FU-Copyright$
 */

#include "StandardTimerProxy.h"

ARA_NAMESPACE_BEGIN

StandardTimerProxy::StandardTimerProxy(TimerType type, void* contextObject) : Timer(type, contextObject) { }

StandardTimerProxy::~StandardTimerProxy(){ }

void StandardTimerProxy::run(unsigned long timeoutInMicroSeconds){
    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());
    clock->scheduleTimer(timerIdentifier, timeoutInMicroSeconds);
}

void StandardTimerProxy::interrupt(){
    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());
    clock->interruptTimer(timerIdentifier);
}

bool StandardTimerProxy::equals(const Timer* otherTimer) const {
    const StandardTimerProxy* otherStandardTimerProxy = dynamic_cast<const StandardTimerProxy*>(otherTimer);
    if (otherStandardTimerProxy == nullptr) {
        return false;
    } 
    return (this->getHashValue() == otherStandardTimerProxy->getHashValue());
}


bool StandardTimerProxy::equals(const std::shared_ptr<Timer> otherTimer) const {
    return this->equals(otherTimer.get());
}

void StandardTimerProxy::notify(){
    this->notifyAllListeners();
}

size_t StandardTimerProxy::getHashValue() const {
    std::hash<void*> voidPtrHash;
    return voidPtrHash(contextObject);
}

void StandardTimerProxy::setTimerIdentifier(unsigned long identifier){
    timerIdentifier = identifier;
}

ARA_NAMESPACE_END
