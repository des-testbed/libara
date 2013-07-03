#include "TestbedTimer.h"

#include <iostream>

TESTBED_NAMESPACE_BEGIN

TestbedTimer::~TestbedTimer(){
   /// TODO: check this (since the thread might not receive the exception)	
   this->interrupt();
}

void TestbedTimer::run(unsigned long timeoutInMicroSeconds){
    try {
        Runner runner;
        std::thread timer(&Runner::run, &runner, timeoutInMicroSeconds);
        timer.join();
        this->notifyAllListeners();
    } catch(ThreadInterruptedException&) {

    }
}

void TestbedTimer::interrupt(){
    throw ThreadInterruptedException();
}

TESTBED_NAMESPACE_END
