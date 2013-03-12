/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTClock.h"
#include "omnetpp/OMNeTTime.h"
#include "omnetpp/OMNeTTimer.h"

using namespace ARA;
using namespace ARA::omnetpp;

OMNeTClock::OMNeTClock(Environment* environment) {
    this->environment = environment;
}

Time* OMNeTClock::makeTime(){
    return new OMNeTTime();
}

Timer* OMNeTClock::getNewTimer() {
    return new OMNeTTimer(++nrOfTimers, this);
}

void OMNeTClock::startTimer(unsigned int timerID, long timeInMilliSeconds) {
    cMessage* newMessage = new OMNeTTimerMessage(timerID);
    environment->scheduleSelfMessage(newMessage, timeInMilliSeconds);
}

void OMNeTClock::handleTimerMessage(OMNeTTimerMessage* msg) {
    EV << "Timer EXPIRED ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
}
