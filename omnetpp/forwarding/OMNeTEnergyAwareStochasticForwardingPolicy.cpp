/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTEnergyAwareStochasticForwardingPolicy.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(OMNeTEnergyAwareStochasticForwardingPolicy);

float OMNeTEnergyAwareStochasticForwardingPolicy::getRandomNumber(){
    return dblrand();
}

void OMNeTEnergyAwareStochasticForwardingPolicy::initialize(){
    setPheromoneWeight(par("alpha").doubleValue());
    setEnergyWeight(par("beta").doubleValue());
}

void OMNeTEnergyAwareStochasticForwardingPolicy::handleMessage(cMessage *msg) {
    throw cRuntimeError("OMNeTEnergyAwareStochasticForwardingPolicy: handleMessage() should never be called!");
}

OMNETARA_NAMESPACE_END
