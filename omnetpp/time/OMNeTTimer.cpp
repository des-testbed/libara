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

void OMNeTTimer::run(unsigned long timeoutInMicroSeconds) {
    clock->startTimer(id, timeoutInMicroSeconds);
}

void OMNeTTimer::interrupt() {
    //TODO implement this
}
