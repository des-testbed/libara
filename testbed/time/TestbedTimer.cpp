#include "TestbedTimer.h"

#include <iostream>

TESTBED_NAMESPACE_BEGIN

TestbedTimer::~TestbedTimer(){
   /// TODO: check this (since the thread might not receive the exception)	
   this->interrupt();
}

void TestbedTimer::run(unsigned long timeoutInMicroSeconds){
    /// this class is defined in the header file of class TestbedTimer
    Runner runner;

    try {
        std::thread timer(&Runner::run, &runner, timeoutInMicroSeconds);
        timer.join();
        this->notifyAllListeners();
    } catch(ThreadInterruptedException&) {
	/// TODO: send exception thread context (better check that)
        runner.interrupt();
    }
}

void TestbedTimer::interrupt(){
    throw ThreadInterruptedException();
}

TESTBED_NAMESPACE_END
