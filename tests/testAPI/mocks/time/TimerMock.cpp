/*
 * $FU-Copyright$
 */

#include "TimerMock.h"

using namespace ARA;

void TimerMock::run(unsigned long timeoutInMicroSeconds) {
    isTimerRunning = true;
}

void TimerMock::interrupt() {
    hasTimerBeenInterrupted = true;
}

void TimerMock::expire() {
    hasTimerExpired = true;
    isTimerRunning = false;
    notifyAllListeners();
}

bool TimerMock::hasExpired() const {
    return hasTimerExpired;
}

bool TimerMock::isRunning() const {
    return isTimerRunning;
}

bool TimerMock::hasBeenInterrupted() const {
    return hasTimerBeenInterrupted;
}
