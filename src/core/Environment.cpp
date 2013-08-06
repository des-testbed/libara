/*
 * $FU-Copyright$
 */

#include "Environment.h"
#include "StandardClock.h"

ARA_NAMESPACE_BEGIN

Environment* Environment::instance = nullptr;

Environment::Environment() {
    clock = new StandardClock();
}

Environment::~Environment() {
    DELETE_IF_NOT_NULL(clock);
}

void Environment::setTheClock(Clock* newClock) {
    delete clock;
    clock = newClock;
}

void Environment::notifyClockHasBeenDeleted() {
    getInstance().clock = nullptr;
}

Environment& Environment::getInstance() {
    if (instance == nullptr) {
       instance = new Environment();
    }
    return *instance;
}

Clock* Environment::getClock() {
    return getInstance().clock;
}

void Environment::setClock(Clock* newClock) {
    getInstance().setTheClock(newClock);
}

ARA_NAMESPACE_END
