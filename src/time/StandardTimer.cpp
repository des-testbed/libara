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
    timer = nullptr;
}

StandardTimer::~StandardTimer(){
    makeSureTimeIsNotRunning();
}

void StandardTimer::makeSureTimeIsNotRunning() {
    if (timerIsRunning) {
        forcefullyStopTimer();
    }

    DELETE_IF_NOT_NULL(timer);
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
    makeSureTimeIsNotRunning();
    timer = new std::thread(&StandardTimer::sleep, this, timeoutInMicroSeconds);
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

    // it is important that we set this to false before we notify the listeners to prevent ressource deadlocks
    timerIsRunning = false;

    if (timeoutInMicroSeconds <= 0) {
        notifyAllListeners();
    }
}

void StandardTimer::interrupt(){
    timerHasBeenInterrupted = true;
}

ARA_NAMESPACE_END
