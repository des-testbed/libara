/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTTimerMessage.h"
#include "omnetpp/Environment.h"

using namespace ARA::omnetpp;

OMNeTTimerMessage::OMNeTTimerMessage(unsigned int timerID) : cMessage("TimerMsg", Environment::TIMER_MSG){
    this->timerID = timerID;
}

unsigned int OMNeTTimerMessage::getTimerID() {
    return timerID;
}
