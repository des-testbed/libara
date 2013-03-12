/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTTimerMessage.h"

using namespace ARA::omnetpp;

OMNeTTimerMessage::OMNeTTimerMessage(unsigned int timerID) : cMessage("TimerMsg"){
    this->timerID = timerID;
}

unsigned int OMNeTTimerMessage::getTimerID() {
    return timerID;
}
