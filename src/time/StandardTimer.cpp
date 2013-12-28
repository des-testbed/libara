/*
 * $FU-Copyright$
 */

#include "StandardTimer.h"

#include <iostream>
#include <system_error>

ARA_NAMESPACE_BEGIN

StandardTimer::StandardTimer(char type, void* contextObject) : Timer(type, contextObject) { }

StandardTimer::~StandardTimer(){ 
    if (timer != nullptr) {
        if (timer->joinable()) {
            timer->join();
        }
    }
}

void StandardTimer::interrupt(){
    std::cout << "[StandardTimer] interrupting timer" << std::endl;
    conditionVariable.notify_all();
}

void StandardTimer::run(unsigned long timeoutInMicroseconds){
    std::cout << "[StandardTimer] starting timer with a timeout of " << timeoutInMicroseconds << " us" << std::endl;
    timer = std::make_shared<std::thread>(&StandardTimer::sleep, this, timeoutInMicroseconds);
}

void StandardTimer::sleep(unsigned long timeoutInMicroseconds){
    std::unique_lock<std::mutex> lock(conditionVariableMutex);

    try {
	if (conditionVariable.wait_for(lock, std::chrono::microseconds(timeoutInMicroseconds)) == std::cv_status::timeout){ 	
            std::cout << "[StandardTimer] timer expired " << std::endl;
	    notifyAllListeners();   
	}
    } catch (const std::system_error& error) {
        std::cerr << "Caught system_error with code " << error.code() << " meaning " << error.what() << std::endl;
    }
}

ARA_NAMESPACE_END
