/*
 * $FU-Copyright$
 */

#include "Timer.h"

using namespace ARA;

Timer::Timer() {
    listeners = std::deque<TimeoutEventListener*>();
}

void Timer::addTimeoutListener(TimeoutEventListener* listener) {
    listeners.push_back(listener);
}

void Timer::notifyAllListeners() {
    for(auto& listener: listeners) {
        listener->timerHasExpired(this);
    }
}
