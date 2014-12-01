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
    isTimerRunning = false;
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

bool TimerMock::equals(const Timer* otherTimer) const {
    const TimerMock* otherTimerMock = dynamic_cast<const TimerMock*>(otherTimer);
    if (otherTimerMock == nullptr) {
        return false;
    } 
    return (this->getHashValue() == otherTimerMock->getHashValue());
}


bool TimerMock::equals(const std::shared_ptr<Timer> otherTimer) const {
    return this->equals(otherTimer.get());
}

size_t TimerMock::getHashValue() const {
    std::hash<void*> voidPtrHash;
    return voidPtrHash(contextObject);
}
