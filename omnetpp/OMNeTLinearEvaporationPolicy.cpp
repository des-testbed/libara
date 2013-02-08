#include "OMNeTLinearEvaporationPolicy.h"

using namespace ARA;

/// The module class needs to be registered with OMNeT++
Define_Module(OMNeTLinearEvaporationPolicy);

//OMNetLinearEvaporationPolicy::OMNetLinearEvaporationPolicy():LinearEvaporationPolicy(new OMNeTTime()){};

void OMNeTLinearEvaporationPolicy::initialize() {
    /// parse the policy parameter from the NED file
    float q = par("q").doubleValue();
    /// parse the delta phi parameter from the NED file
    float threshold = par("threshold").doubleValue();
}

void OMNeTLinearEvaporationPolicy::handleMessage(cMessage *msg) {
    /// this method is a stub since we don't handle messages in the evaporation policy
}
