/*
 * $FU-Copyright$
 */

#include "StandardTimer.h"
#include "ThreadInterruptedException.h"

#include <iostream>
#include <system_error>

ARA_NAMESPACE_BEGIN

StandardTimer::StandardTimer(char type, void* contextObject) : Timer(type, contextObject) { }

StandardTimer::~StandardTimer(){ }

void StandardTimer::interrupt(){
    conditionVariable.notify_all();
}

void StandardTimer::run(unsigned long timeoutInMicroseconds){
    std::thread thread(&StandardTimer::sleep, this, timeoutInMicroseconds);
    thread.join();
}

void StandardTimer::sleep(unsigned long timeoutInMicroseconds){
    std::unique_lock<std::mutex> lock(conditionVariableMutex);

    try {
		if(conditionVariable.wait_for(lock, std::chrono::microseconds(timeoutInMicroseconds)) == std::cv_status::timeout){ 
		  notifyAllListeners();   
		}
    } catch (const std::system_error& error) {
       std::cout << "Caught system_error with code " << error.code() 
                  << " meaning " << error.what() << '\n';
    }
}

ARA_NAMESPACE_END
