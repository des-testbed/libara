/*
 * $FU-Copyright$
 */

#include "omnetpp/Environment.h"
#include "omnetpp/OMNeTClock.h"

using namespace ARA::omnetpp;

Define_Module(ARA::omnetpp::Environment);

void Environment::initialize() {
    ARA::Environment::setClock(new OMNeTClock());
}
