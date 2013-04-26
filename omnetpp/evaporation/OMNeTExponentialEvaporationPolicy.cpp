/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTExponentialEvaporationPolicy.h"

OMNETARA_NAMESPACE_BEGIN

// The module class needs to be registered with OMNeT++
Define_Module(OMNeTExponentialEvaporationPolicy);

OMNeTExponentialEvaporationPolicy::OMNeTExponentialEvaporationPolicy() : ExponentialEvaporationPolicy(0.9, 1000, 0) {
    // any parameters need to be initialized in OMNeTExponentialEvaporationPolicy::initialize()
}

void OMNeTExponentialEvaporationPolicy::initialize() {
    evaporationFactor = par("evaporationFactor").doubleValue();
    timeInterval = par("timeInterval").doubleValue();
    minimumTimeDifferenceBeforeEvaporation = par("minTimeDifferenceBeforeEvaporation").doubleValue();
    threshold = par("threshold").doubleValue();
}

void OMNeTExponentialEvaporationPolicy::handleMessage(cMessage* msg) {
    throw cRuntimeError("OMNeTExponentialEvaporationPolicy: handleMessage() should never be called!");
}

OMNETARA_NAMESPACE_END
