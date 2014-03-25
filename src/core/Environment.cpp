/*
 * $FU-Copyright$
 */

#include "Environment.h"
#include "DummyClock.h"
#include "StandardClock.h"

ARA_NAMESPACE_BEGIN

Environment* Environment::instance = nullptr;

Environment::Environment() {
    clock = new StandardClock();
//    clock = new DummyClock();
}

Environment::~Environment() {
    DELETE_IF_NOT_NULL(clock);
}

void Environment::setTheClock(Clock* newClock) {
    DELETE_IF_NOT_NULL(clock);
    clock = newClock;
}

void Environment::notifyClockHasBeenDeleted() {
    getInstance().clock = nullptr;
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

ARA_NAMESPACE_END
