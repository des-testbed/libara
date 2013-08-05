#include "TestbedTimer.h"

#include <iostream>
#include <system_error>


TESTBED_NAMESPACE_BEGIN

TestbedTimer::TestbedTimer(char type, void* contextObject) : Timer(type, contextObject) {
    this->active = false;
}

TestbedTimer::~TestbedTimer(){
    if (this->active) {
        this->active = false;
        std::cout << "destructor has been called" << std::endl;
/*
        if(this->timer->joinable()){
            std::cout << "thread is joinable" << std::endl;
            try {
                this->timer->join();
            } catch (const std::system_error& e) {
                std::cout << "Caught system_error with code " << e.code()  << " meaning " << e.what() << '\n';
            }
            std::cout << "thread has rejoined main scope" << std::endl;
        }else{
            std::cout << "no need to join" << std::endl;
        }

       if(timer != nullptr) {
           delete timer;
       } */
    }
}

void TestbedTimer::run(unsigned long timeoutInMicroSeconds){
    this->active = true;
    //timer = new std::thread(&TestbedTimer::sleep, this, timeoutInMicroSeconds);
    std::thread timer(&TestbedTimer::sleep, this, timeoutInMicroSeconds);
    timer.detach();

}

void TestbedTimer::sleep(unsigned long timeoutInMicroSeconds){
    long interval = 500000;

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
}

void TestbedTimer::interrupt(){
    this->active = false;
    //std::cout << "interrupt has been called" << std::endl;
    //this->timer->join();
    //std::cout << "thread has rejoined main scope" << std::endl;
    //delete timer;
}

TESTBED_NAMESPACE_END
