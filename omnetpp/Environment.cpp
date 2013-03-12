/*
 * $FU-Copyright$
 */

#include "omnetpp/Environment.h"
#include "omnetpp/OMNeTClock.h"

using namespace ARA::omnetpp;

Define_Module(ARA::omnetpp::Environment);

void Environment::initialize() {
    ARA::Environment::setClock(new OMNeTClock(this));
}

void Environment::handleMessage(cMessage* msg) {
    if(msg->getKind() == MessageTypes::TIMER_MSG) {
        OMNeTTimerMessage* timerMessage = (OMNeTTimerMessage*) msg;
        OMNeTClock* clock = (OMNeTClock*) ARA::Environment::getClock();
        clock->handleTimerMessage(timerMessage);
    }
}

void Environment::scheduleSelfMessage(cMessage* msg, long timeInMilliSeconds) {
    // take ownership of the message
    take(msg);

    // this will be called from the OMNeTARAClient simple module so we need to explicitly declare we are switching the context
    Enter_Method("scheduleSelfMessage");

    // transform the time to the wanted time scale resolution
    SimTime currentTime = simTime();
    double timeDelta = timeInMilliSeconds * pow(10, SimTime::SCALEEXP_MS + currentTime.getScaleExp());

    // finally schedule the self message
    scheduleAt(currentTime + timeDelta , msg);
}
