/*
 * $FU-Copyright$
 */

#include "Timer.h"

#include <thread>
#include <system_error>
    

using namespace ARA;

Timer::Timer(char type, void* contextObject) {
    this->type = type;
    this->contextObject = contextObject;
    listeners = std::deque<TimeoutEventListener*>();
}

char Timer::getType() const {
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
    // DEBUG: std::cerr << "[Timer::notifyAllListeners] for " <<  TimerType::getAsString(type) << std::endl;

    for(auto& listener: listeners) {
	    std::weak_ptr<Timer> timer = this->getTimer();

        try {
            listener->timerHasExpired(timer);
        } catch (const std::system_error& error) {
	        std::cerr << "[Timer] caught system_error " << std::endl;
            std::cerr << "Error:    " << error.what() << std::endl;
            std::cerr << "Code:     " << error.code().value() << std::endl;
            std::cerr << "Category: " << error.code().category().name() << std::endl;
            std::cerr << "Message:  " << error.code().message() << std::endl;
        }
    }
}
