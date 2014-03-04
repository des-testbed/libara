/*
 * $FU-Copyright$
 */

#include "Timer.h"

using namespace ARA;

Timer::Timer(TimerType type, void* contextObject) {
    this->type = type;
    this->contextObject = contextObject;
    listeners = std::deque<TimeoutEventListener*>();
}

TimerType Timer::getType() const {
    return type;
}

void* Timer::getContextObject() {
    return contextObject;
}

void Timer::setContextObject(void* contextObject) {
    this->contextObject = contextObject;
}

void Timer::addTimeoutListener(TimeoutEventListener* listener) {
    listeners.push_back(listener);
}

void Timer::notifyAllListeners() {
    for(auto& listener: listeners) {
        listener->timerHasExpired(this);
    }
}
