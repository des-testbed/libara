/*
 * $FU-Copyright$
 */

#include "StandardTimer.h"
#include "StackTrace.h"

ARA_NAMESPACE_BEGIN

StandardTimer::StandardTimer(char type, void* contextObject) : Timer(type, contextObject) { 
  printStacktrace(10);
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
    conditionVariable.notify_all();
}

void StandardTimer::setCallback(std::shared_ptr<StandardTimerProxy> proxy){
    this->callback = proxy;
}

void StandardTimer::sleep(unsigned long timeoutInMicroseconds){
    std::unique_lock<std::mutex> lock(conditionVariableMutex);

    try {
        if (conditionVariable.wait_for(lock, std::chrono::microseconds(timeoutInMicroseconds)) == std::cv_status::timeout){
	        callback->notify();
            // DEBUG:
            std::cerr << "callback of timer type " << TimerType::getAsString(type) << " called" << std::endl;
	    }
    } catch (const std::system_error& error) {
	    std::cerr << "[StandardTimer] Caught system_error in " << TimerType::getAsString(type) << std::endl;
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
