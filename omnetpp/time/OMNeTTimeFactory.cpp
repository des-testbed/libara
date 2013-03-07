#include "OMNeTTimeFactory.h"
#include "OMNeTTime.h"
#include "OMNeTTimer.h"

using namespace ARA;
using namespace ARA::omnetpp;

Time* OMNeTTimeFactory::makeTime(){
    return new OMNeTTime();
}

Timer* OMNeTTimeFactory::getNewTimer() {
    return new OMNeTTimer();
}
