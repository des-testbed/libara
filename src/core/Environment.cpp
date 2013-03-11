/*
 * $FU-Copyright$
 */

#include "Environment.h"
#include "UnixClock.h"

namespace ARA {

Environment::Environment() {
    clock = new UnixClock();
}

Environment::~Environment() {
    delete clock;
}

void Environment::setTheClock(Clock* newClock) {
    delete clock;
    clock = newClock;
}

Environment& Environment::getInstance() {
    static Environment instance;
    return instance;
}

Clock* Environment::getClock() {
    return getInstance().clock;
}

void Environment::setClock(Clock* newClock) {
    getInstance().setTheClock(newClock);
}

} /* namespace ARA */
