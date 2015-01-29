/*
 * $FU-Copyright$
 */

#include "StandardTimerProxy.h"

ARA_NAMESPACE_BEGIN

StandardTimerProxy::StandardTimerProxy(char type, void* contextObject) : Timer(type, contextObject) { 
    try {
        logger = spdlog::get("file_logger");
    } catch (const spdlog::spdlog_ex& exception) {
        std::cerr<< "getting file logger failed: " << exception.what() << std::endl;
    }

    logger->trace() << "[StandardTimerProxy::StandardTimerProxy] " << "proxy for timer of type " << TimerType::getAsString(type) << " initialized";
}

StandardTimerProxy::~StandardTimerProxy(){ }

void StandardTimerProxy::run(unsigned long timeoutInMicroSeconds){
    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());
    
    if (clock) {
        clock->scheduleTimer(timerIdentifier, timeoutInMicroSeconds);
    } else {
        logger->error() << "[StandardTimerProxy::run] " << "dynamic cast failed!";
    }
}

void StandardTimerProxy::interrupt(){
    logger->trace() << "[StandardTimerProxy::interrupt] " << "interrupt timer " <<  timerIdentifier;

    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());

    if (clock) {
        clock->interruptTimer(timerIdentifier);
    } else {
        logger->error() << "[StandardTimerProxy::interrupt] " << "dynamic cast failed!";
    }
}

bool StandardTimerProxy::equals(const Timer* otherTimer) const {
    const StandardTimerProxy* otherStandardTimerProxy = dynamic_cast<const StandardTimerProxy*>(otherTimer);

    if (otherStandardTimerProxy) {
        return (this->getHashValue() == otherStandardTimerProxy->getHashValue());
    } else {
        logger->error() << "[StandardTimerProxy::equals] " << "dynamic cast failed!";
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
