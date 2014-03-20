/*
 * $FU-Copyright$
 */

#include "StandardTimer.h"

ARA_NAMESPACE_BEGIN

StandardTimer::StandardTimer(char type, void* contextObject) : Timer(type, contextObject) { }

StandardTimer::~StandardTimer(){ }

void StandardTimer::run(unsigned long timeoutInMicroSeconds){
    std::function<void()> timer = std::bind(&StandardTimer::sleep, this, timeoutInMicroSeconds);

    StandardClock* clock = dynamic_cast<StandardClock*>(Environment::getClock());
    clock->scheduleTimer(timer);
}

void StandardTimer::interrupt(){
    conditionVariable.notify_all();
}

void StandardTimer::sleep(unsigned long timeoutInMicroseconds){
    std::unique_lock<std::mutex> lock(conditionVariableMutex);

    try {
	if(conditionVariable.wait_for(lock, std::chrono::microseconds(timeoutInMicroseconds)) == std::cv_status::timeout){
            // std::cout << "[StandardTimer] timer expired " << std::endl;
            notifyAllListeners();
	}
    } catch (const std::system_error& error) {
	std::cerr << "Caught system_error with code " << error.code() << " meaning " << error.what() << std::endl;
    }
}

ARA_NAMESPACE_END
