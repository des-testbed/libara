/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTTimer.h"

using namespace ARA;
using namespace ARA::omnetpp;

OMNeTTimer::OMNeTTimer(unsigned int id, OMNeTClock* clock) {
    this->id = id;
    this->clock = clock;
}

void OMNeTTimer::run(long timeInMilliSeconds) {
    clock->startTimer(id, timeInMilliSeconds);
}

void OMNeTTimer::interrupt() {
    //TODO implement this
}
