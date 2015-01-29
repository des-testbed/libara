/*
 * $FU-Copyright$
 */

#include "StandardTimer.h"
#include "StackTrace.h"

ARA_NAMESPACE_BEGIN

StandardTimer::StandardTimer(char type, void* contextObject) : Timer(type, contextObject) { 
    interrupted = false;

    try {
        logger = spdlog::get("file_logger");
    } catch (const spdlog::spdlog_ex& exception) {
        std::cerr<< "getting file logger failed: " << exception.what() << std::endl;
    }

    logger->trace() << "[StandardTimer::StandardTimer] " << "timer of type " << TimerType::getAsString(type) << " initialized";
}

StandardTimer::~StandardTimer(){ }

void StandardTimer::run(unsigned long timeoutInMicroSeconds){
    std::function<void()> timer = std::bind(&StandardTimer::sleep, this, timeoutInMicroSeconds);

    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());

    if (clock) {
        clock->scheduleTimer(timer);
    } else {
        // DEBUG:
        logger->error() << "[StandardTimer::run] " << "dynamic cast failed!";
    }
}

void StandardTimer::interrupt(){
    interrupted = true;
    logger->trace() << "[StandardTimer::interrupt] " << "interrupt timer";
    conditionVariable.notify_all();
}

void StandardTimer::setCallback(std::shared_ptr<StandardTimerProxy> proxy){
    this->callback = proxy;
}

void StandardTimer::sleep(unsigned long timeoutInMicroseconds){
    std::unique_lock<std::mutex> lock(conditionVariableMutex);
    std::chrono::microseconds timeout(timeoutInMicroseconds);

    try {
        std::cv_status result;

        if ((result = conditionVariable.wait_for(lock, timeout)) == std::cv_status::timeout){
            if (!interrupted) {
	            callback->notify();
            }

            logger->trace() << "[StandardTimer::sleep] " << "callback of timer type " << TimerType::getAsString(type) << " called";
        } else if (result == std::cv_status::no_timeout) {
            logger->trace() << "[StandardTimer::sleep] " << "callback of timer type " << TimerType::getAsString(type) << " cancelled";
        } else {
            logger->error() << "[StandardTimer::sleep] " << "don't know what happened to " << TimerType::getAsString(type);
        }
    } catch (const std::system_error& error) {
	    logger->error() << "[StandardTimer] caught system_error in " << TimerType::getAsString(type);
        logger->error() << " Error:    " << error.what();
        logger->error() << " Code:     " << error.code().value();
        logger->error() << " Category: " << error.code().category().name();
        logger->error() << " Message:  " << error.code().message();
    }
}

bool StandardTimer::equals(const Timer* otherTimer) const {
    const StandardTimer* otherStandardTimer = dynamic_cast<const StandardTimer*>(otherTimer);

    if (otherStandardTimer == nullptr) {
        return false;
    }

    return (this->getHashValue() == otherStandardTimer->getHashValue());
}


bool StandardTimer::equals(const std::shared_ptr<Timer> otherTimer) const {
    return this->equals(otherTimer.get());
}

size_t StandardTimer::getHashValue() const {
    std::hash<void*> voidPtrHash;
    return voidPtrHash(contextObject);
}

ARA_NAMESPACE_END
