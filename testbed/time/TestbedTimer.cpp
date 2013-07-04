#include "TestbedTimer.h"

#include <iostream>

TESTBED_NAMESPACE_BEGIN

TestbedTimer::~TestbedTimer(){
   /// TODO: check this (since the thread might not receive the exception)	
///   this->interrupt();
}

void TestbedTimer::run(unsigned long timeoutInMicroSeconds){
    std::thread timer(&TestbedTimer::sleep, this, timeoutInMicroSeconds);
    timer.detach();
}

void TestbedTimer::sleep(unsigned long timeoutInMicroSeconds){
    try {
        this->notifyAllListeners();
        /// set the sleep time
        std::chrono::microseconds duration(timeoutInMicroSeconds);
        /// set thread to sleep
        std::this_thread::sleep_for(duration);
    } catch(ThreadInterruptedException&) {

    }
}

void TestbedTimer::interrupt(){
    throw ThreadInterruptedException();
}

TESTBED_NAMESPACE_END
