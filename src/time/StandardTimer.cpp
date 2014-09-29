/*
 * $FU-Copyright$
 */

#include "StandardTimer.h"
#include "StackTrace.h"

ARA_NAMESPACE_BEGIN

StandardTimer::StandardTimer(char type, void* contextObject) : Timer(type, contextObject) { 
   interrupted = false;
}

StandardTimer::~StandardTimer(){ }

void StandardTimer::run(unsigned long timeoutInMicroSeconds){
    std::function<void()> timer = std::bind(&StandardTimer::sleep, this, timeoutInMicroSeconds);

    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());

    if (clock) {
        clock->scheduleTimer(timer);
    } else {
        // DEBUG:
        std::cerr << "[StandardTimer::run] dynamic cast failed!" << std::endl;
    }
}

void StandardTimer::interrupt(){
    interrupted = true;
    // DEBUG: std::cerr << "[StandardTimer::interrupt] interrupt timer" << std::endl;
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
            // DEBUG: 
            std::cerr << "[StandardTimer] callback of timer type " << TimerType::getAsString(type) << " called" << std::endl;
        } else if (result == std::cv_status::no_timeout) {
            // DEBUG: 
            std::cerr << "[StandardTimer] callback of timer type " << TimerType::getAsString(type) << " cancelled " << std::endl;
        } else {
            // DEBUG: 
            std::cerr << "[StandardTimer] don't know what happened to " << TimerType::getAsString(type) << std::endl;
        }
    } catch (const std::system_error& error) {
	    std::cerr << "[StandardTimer] caught system_error in " << TimerType::getAsString(type) << std::endl;
        std::cerr << "Error:    " << error.what() << std::endl;
        std::cerr << "Code:     " << error.code().value() << std::endl;
        std::cerr << "Category: " << error.code().category().name() << std::endl;
        std::cerr << "Message:  " << error.code().message() << std::endl;
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
