/*
 * $FU-Copyright$
 */

#include "Environment.h"
#include "DummyClock.h"
#include "StandardClock.h"

ARA_NAMESPACE_BEGIN

Environment* Environment::instance = nullptr;

Environment::Environment() {
    /**
     * The DummyClock is basically a StandardClock without thread support. However,
     * the thread support in the StandardClock is necessary in order to have working
     * timers. Here, the StandardClock holds a set of threads in a pool which handle the 
     * task of setting a timer, notifiyng a listenere if a timer has expired, ...
     *
     * We introduced the class DummyClock in order to run tests. If you write
     * a new ant routing algorithm you have to make sure that you set clock via the
     * Environment class to the StandardClock or your own timer implementation!
     */
    clock = new DummyClock();
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
