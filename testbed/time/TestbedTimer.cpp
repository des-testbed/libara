#include "TestbedTimer.h"

#include <iostream>
#include <system_error>


TESTBED_NAMESPACE_BEGIN

TestbedTimer::TestbedTimer(char type, void* contextObject) : Timer(type, contextObject) {
    this->active = false;
    this->timerIsRunning = false;
    this->timer = nullptr;
}

TestbedTimer::~TestbedTimer(){
    if (this->active) {
        this->active = false;

        if(this->timerIsRunning){

            try {
                this->timer->join();
            } catch (const std::system_error& e) {
                std::cout << "Caught system_error with code " << e.code()  << " meaning " << e.what() << '\n';
            }

        }

       if(timer != nullptr) {
           delete timer;
       }
    }
}

void TestbedTimer::run(unsigned long timeoutInMicroSeconds){
    if (this->timerIsRunning) {
       this->active = false;
       this->timer->join();
    }
   
    if (this->timer != nullptr) {
        delete timer;
    }
    timer = new std::thread(&TestbedTimer::sleep, this, timeoutInMicroSeconds);
}

void TestbedTimer::sleep(unsigned long timeoutInMicroSeconds){
    this->active = true;
    this->timerIsRunning = true;
    unsigned long interval = 500000;

    if(timeoutInMicroSeconds < interval){
        interval = timeoutInMicroSeconds;
    }

    while((timeoutInMicroSeconds > 0) && (active == true)){
        if (active) {
            timeoutInMicroSeconds -= interval;
            /// set the sleep time
            std::chrono::microseconds duration(interval);
            /// set thread to sleep
            std::this_thread::sleep_for(duration);
        }
    }

    if (active) {
       this->notifyAllListeners();
    } 

    this->timerIsRunning = false;
}

void TestbedTimer::interrupt(){
    this->active = false;
}

TESTBED_NAMESPACE_END
