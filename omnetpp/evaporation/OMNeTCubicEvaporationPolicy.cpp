/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTCubicEvaporationPolicy.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(OMNeTCubicEvaporationPolicy);

void OMNeTCubicEvaporationPolicy::initialize() {
    // reads the member variables from the ned file
    this->setTimeUntilPlateauCenterIsReached(par("timeUntilPlateauCenterIsReached").doubleValue());
    this->setPlateauCenterDepth(par("plateauCenterDepth").doubleValue());
    this->setExponent(par("exponent").doubleValue());
}

void OMNeTCubicEvaporationPolicy::handleMessage(cMessage* message) {
    throw cRuntimeError("OMNeTCubicEvaporationPolicy is not supposed to handle any messages");
}

OMNETARA_NAMESPACE_END
