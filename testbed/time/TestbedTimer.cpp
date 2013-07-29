#include "TestbedTimer.h"

#include <iostream>

TESTBED_NAMESPACE_BEGIN

TestbedTimer::TestbedTimer(char type, void* contextObject) : Timer(type, contextObject) {
    this->active = false;
}

TestbedTimer::~TestbedTimer(){
    if (this->active) {
        this->active = false;
        this->timer.join();
    }
}

void TestbedTimer::run(unsigned long timeoutInMicroSeconds){
    this->active = true;
    timer = std::thread(&TestbedTimer::sleep, this, timeoutInMicroSeconds);
}

void TestbedTimer::sleep(unsigned long timeoutInMicroSeconds){
    int interval = 500000;

    while(timeoutInMicroSeconds > 0){
        if (active) {
            timeoutInMicroSeconds -= interval;
            /// set the sleep time
            std::chrono::microseconds duration(interval);
            /// set thread to sleep
            std::this_thread::sleep_for(duration);
        } else {
            break;
        }
    }

    if (active) {
       this->notifyAllListeners();
    } 
}

void TestbedTimer::interrupt(){
    this->active = false;
    this->timer.join();
}

TESTBED_NAMESPACE_END
