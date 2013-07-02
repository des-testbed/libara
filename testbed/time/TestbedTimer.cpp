#include "TestbedTimer.h"

#include <iostream>

TESTBED_NAMESPACE_BEGIN

TestbedTimer::TestbedTimer(){

}

TestbedTimer::~TestbedTimer(){
//   this->interrupt();
}

void TestbedTimer::run(unsigned long timeoutInMicroSeconds){
   try {
       /// set the sleep time
       std::chrono::microseconds duration(timeoutInMicroSeconds);
       /// set thread to sleep
       std::this_thread::sleep_for(duration);
       std::cout << " foo " << std::endl;
       /// notify listeners 
       this->notifyAllListeners();
   } catch (ThreadInterruptedException&) {
       /// do something smart
   }
}

void TestbedTimer::interrupt(){
   throw ThreadInterruptedException();
}

TESTBED_NAMESPACE_END
