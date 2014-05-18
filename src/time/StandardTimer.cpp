/*
 * $FU-Copyright$
 */

#include "StandardTimer.h"

ARA_NAMESPACE_BEGIN

StandardTimer::StandardTimer(TimerType type, void* contextObject) : Timer(type, contextObject) { }

StandardTimer::~StandardTimer(){ }

void StandardTimer::run(unsigned long timeoutInMicroSeconds){
    std::function<void()> timer = std::bind(&StandardTimer::sleep, this, timeoutInMicroSeconds);

    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());
    clock->scheduleTimer(timer);
}

void StandardTimer::interrupt(){
    conditionVariable.notify_all();
}

void StandardTimer::setCallback(std::weak_ptr<StandardTimerProxy> proxy){
    this->callback = proxy;
}

void StandardTimer::sleep(unsigned long timeoutInMicroseconds){
    std::unique_lock<std::mutex> lock(conditionVariableMutex);

    try {
	    if (conditionVariable.wait_for(lock, std::chrono::microseconds(timeoutInMicroseconds)) == std::cv_status::timeout){
	        auto proxy = callback.lock();

	        if (proxy) {
	            proxy->notify();
	        } else {
	            std::cerr << "shared_ptr expired and hence, no object to call for expired timer " << std::endl;
	        }
	    }
    } catch (const std::system_error& error) {
	    std::cerr << "Caught system_error with code " << error.code() << " meaning " << error.what() << std::endl;
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
