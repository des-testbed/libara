/*
 * $FU-Copyright$
 */

#include "Timer.h"

#include <thread>

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
    std::thread::id this_id = std::this_thread::get_id();
    std::cerr << "[Timer::notifyAllListeners] for timer type " << type << " and thread id " << this_id << " and object " << this << std::endl;
    for(auto& listener: listeners) {
	    std::weak_ptr<Timer> timer = this->getTimer();
        listener->timerHasExpired(timer);
    }
}
