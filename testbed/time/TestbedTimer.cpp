#include "TestbedTimer.h"

#include <iostream>

TESTBED_NAMESPACE_BEGIN

TestbedTimer::TestbedTimer(){
    this->active = false;
}

TestbedTimer::~TestbedTimer(){
    this->mutex.lock(); 
    if (this->active) {
        this->active = false;
        this->mutex.unlock(); 
        this->timer.join();
    } else {
        this->mutex.unlock(); 
    }
}

void TestbedTimer::run(unsigned long timeoutInMicroSeconds){
    this->active = true;
    timer = std::thread(&TestbedTimer::sleep, this, timeoutInMicroSeconds);
}

void TestbedTimer::sleep(unsigned long timeoutInMicroSeconds){
    /// determine the sleep intervals
    int interval = timeoutInMicroSeconds / 10;

    while(timeoutInMicroSeconds > 0){

        this->mutex.lock(); 
        if (active) {
            this->mutex.unlock(); 
            timeoutInMicroSeconds -= interval;
            /// set the sleep time
            std::chrono::microseconds duration(interval);
            /// set thread to sleep
            std::this_thread::sleep_for(duration);
        } else {
            this->mutex.unlock(); 
            break;
        }
    }

    this->mutex.lock(); 
    if (active) {
       this->mutex.unlock(); 
       this->notifyAllListeners();
    } else {
       this->mutex.unlock(); 
    }
}

void TestbedTimer::interrupt(){
    this->mutex.lock(); 
    this->active = false;
    this->mutex.unlock(); 
}

TESTBED_NAMESPACE_END
