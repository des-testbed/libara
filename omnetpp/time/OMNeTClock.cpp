/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTClock.h"
#include "omnetpp/OMNeTTime.h"
#include "omnetpp/OMNeTTimer.h"

using namespace ARA;
using namespace ARA::omnetpp;

Time* OMNeTClock::makeTime(){
    return new OMNeTTime();
}

Timer* OMNeTClock::getNewTimer() {
    return new OMNeTTimer();
}
