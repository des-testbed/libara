#include "OMNeTClock.h"
#include "OMNeTTime.h"
#include "OMNeTTimer.h"

using namespace ARA;
using namespace ARA::omnetpp;

Time* OMNeTClock::makeTime(){
    return new OMNeTTime();
}

Timer* OMNeTClock::getNewTimer() {
    return new OMNeTTimer();
}
