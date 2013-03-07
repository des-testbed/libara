/*
 * $FU-Copyright$
 */

#include "TimerMock.h"

using namespace ARA;

void TimerMock::run(double timeInMilliSeconds) {
    isTimerRunning = true;
}

void TimerMock::expire() {
    notifyAllListeners();
    hasTimerExpired = true;
    isTimerRunning = false;
}

bool TimerMock::hasExpired() const {
    return hasTimerExpired;
}

bool TimerMock::isRunning() const {
    return isTimerRunning;
}
