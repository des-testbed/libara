/*
 * $FU-Copyright$
 */

#include "StandardTimer.h"
#include "ThreadInterruptedException.h"

#include <iostream>
#include <system_error>

ARA_NAMESPACE_BEGIN

StandardTimer::StandardTimer(char type, void* contextObject) : Timer(type, contextObject) {
    timerHasBeenInterrupted = false;
    timerIsRunning = false;
}

StandardTimer::~StandardTimer(){
/*
    if (isCalledFromTimerThreadContext() == false) {
        makeSureTimeIsNotRunning();
        DELETE_IF_NOT_NULL(timer);
    } else {
        // FIXME will cause a memory leak for timer
        // After the destructor will be finished, the this pointer for StandardTimer::sleep will be invalidated.
        // I am not sure if this will result in undefined behavior :/
    }
*/
}

bool StandardTimer::isCalledFromTimerThreadContext() {
    return timer != nullptr && std::this_thread::get_id() == timer->get_id();
}

void StandardTimer::makeSureTimeIsNotRunning() {
    if (timerIsRunning) {
        forcefullyStopTimer();
    }
}

void StandardTimer::forcefullyStopTimer() {
    try {
        interrupt();
        timer->join();
    } catch (const std::system_error& e) {
        std::cout << "Caught system_error with code " << e.code()  << " meaning " << e.what() << '\n';
    }
}

void StandardTimer::run(unsigned long timeoutInMicroSeconds){
    if (isCalledFromTimerThreadContext()) {
        sleep(timeoutInMicroSeconds);
    } else {
        makeSureTimeIsNotRunning();
//        DELETE_IF_NOT_NULL(timer);
        timer = std::make_shared<std::thread>(&StandardTimer::sleep, this, timeoutInMicroSeconds);
//        timer = new std::thread(&StandardTimer::sleep, this, timeoutInMicroSeconds);
    }
}

void StandardTimer::sleep(unsigned long timeoutInMicroSeconds){
    timerHasBeenInterrupted = false;
    timerIsRunning = true;
    unsigned long interruptionCheckInterval = 500000;

    if (timeoutInMicroSeconds < interruptionCheckInterval) {
        interruptionCheckInterval = timeoutInMicroSeconds;
    }

    while ((timeoutInMicroSeconds > 0) && (timerHasBeenInterrupted == false)) {
        timeoutInMicroSeconds -= interruptionCheckInterval;
        std::chrono::microseconds duration(interruptionCheckInterval);
        std::this_thread::sleep_for(duration);
    }

    if (timeoutInMicroSeconds <= 0) {
        notifyAllListeners();
    }

    timerIsRunning = false;
}

void StandardTimer::interrupt(){
    timerHasBeenInterrupted = true;
}

ARA_NAMESPACE_END
