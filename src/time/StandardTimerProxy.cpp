/*
 * $FU-Copyright$
 */

#include "StandardTimerProxy.h"

ARA_NAMESPACE_BEGIN

StandardTimerProxy::StandardTimerProxy(char type, void* contextObject) : Timer(type, contextObject) { }

StandardTimerProxy::~StandardTimerProxy(){ }

void StandardTimerProxy::run(unsigned long timeoutInMicroSeconds){
    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());
    
    if (clock) {
        clock->scheduleTimer(timerIdentifier, timeoutInMicroSeconds);
    } else {
        /// DEBUG
        std::cerr << "[StandardTimerProxy::run] dynamic cast failed!" << std::endl;
    }
}

void StandardTimerProxy::interrupt(){
    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());

    if (clock) {
        clock->interruptTimer(timerIdentifier);
    } else {
        /// DEBUG
        std::cerr << "[StandardTimerProxy::interrupt] dynamic cast failed!" << std::endl;
    }
}

bool StandardTimerProxy::equals(const Timer* otherTimer) const {
    const StandardTimerProxy* otherStandardTimerProxy = dynamic_cast<const StandardTimerProxy*>(otherTimer);

    if (otherStandardTimerProxy) {
        return (this->getHashValue() == otherStandardTimerProxy->getHashValue());
    } else {
        /// DEBUG
        std::cerr << "[StandardTimerProxy::equals] dynamic cast failed!" << std::endl;
    }

    return false;
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

unsigned long StandardTimerProxy::getTimerIdentifier(){
    return timerIdentifier;
}

ARA_NAMESPACE_END
