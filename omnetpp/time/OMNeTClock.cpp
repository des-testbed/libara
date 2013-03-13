/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTClock.h"
#include "omnetpp/OMNeTTime.h"
#include "omnetpp/OMNeTTimer.h"
#include "Environment.h"

using namespace ARA;
using namespace ARA::omnetpp;

Define_Module(ARA::omnetpp::OMNeTClock);

void OMNeTClock::initialize() {
    if(isInitialized == false) {
        ARA::Environment::setClock(this);
        isInitialized = true;
    }
}

Time* OMNeTClock::makeTime(){
    return new OMNeTTime();
}

Timer* OMNeTClock::getNewTimer() {
    unsigned int timerID = timerIDCounter++;
    runningTimers[timerID] = new OMNeTTimer(timerID, this);
    return runningTimers[timerID];
}

void OMNeTClock::startTimer(unsigned int timerID, unsigned long timeoutInMicroSeconds) {
    cMessage* newMessage = new OMNeTTimerMessage(timerID);
    take(newMessage);

    // this will be called from the OMNeTARAClient simple module so we need to explicitly declare we are switching the context
    Enter_Method_Silent("OMNeTClock::startTimer");

    // transform the time to the wanted time scale resolution
    SimTime currentTime = simTime();
    int64 timeDeltaRaw = timeoutInMicroSeconds * pow(10, SimTime::SCALEEXP_US - currentTime.getScaleExp());

    SimTime scheduledTime = SimTime();
    scheduledTime.setRaw(currentTime.raw() + timeDeltaRaw);

    // store the message so we can abort it later if necessary
    pendingSelfMessages[timerID] = newMessage;

    // finally schedule the self message
    scheduleAt(scheduledTime, newMessage);

    EV << "Timer " << timerID << " has been started\n";
}

void OMNeTClock::stopTimer(unsigned int timerID) {
    cancelTimerEvents(timerID);
    runningTimers.erase(timerID);
    EV << "Timer " << timerID << " has been stopped\n";
}

void OMNeTClock::timerHasBeenDeleted(unsigned int timerID) {
    cancelTimerEvents(timerID);
    runningTimers.erase(timerID);
    EV << "Timer " << timerID << " has been deleted\n";
}

void OMNeTClock::cancelTimerEvents(unsigned int timerID) {
    std::unordered_map<unsigned int, cMessage*>::iterator pendingSelfMessage = pendingSelfMessages.find(timerID);
    if (pendingSelfMessage != pendingSelfMessages.end()) {
        cancelEvent(pendingSelfMessage->second);
        pendingSelfMessages.erase(timerID);
    }
}

void OMNeTClock::handleMessage(cMessage* msg) {
    OMNeTTimerMessage* timerMessage = (OMNeTTimerMessage*) msg;
    unsigned int timerID = timerMessage->getTimerID();
    EV << "Timer " << timerID << " has timed out\n";

    // dispatch the message
    OMNeTTimer* expiredTimer = runningTimers[timerID];
    expiredTimer->notifyTimeExpired();

    // remove the timer and pending self messages from the local maps
    runningTimers.erase(timerID);
    if (pendingSelfMessages.find(timerID) != pendingSelfMessages.end()) {
        pendingSelfMessages.erase(timerID);
    }
    delete msg;
}
