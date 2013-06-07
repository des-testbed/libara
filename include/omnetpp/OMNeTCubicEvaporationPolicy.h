/*
 * $FU-Copyright$
 */

#ifndef OMNET_LINEAR_EVAPORATION_POLICY_H_
#define OMNET_LINEAR_EVAPORATION_POLICY_H_

#include "OMNeTARAMacros.h"
#include "CubicEvaporationPolicy.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * This class provides the cubic evaporation function of the ant routing algorithm (ARA)
 * for the OMNeT++ simulation framework.
 */
class OMNeTCubicEvaporationPolicy : public CubicEvaporationPolicy, public cSimpleModule {
    public:

        /**
         * Creates a new instance of OMNeTCubicEvaporationPolicy.
         * Please note that the parameters of the base class are chosen arbitrary
         * to supply a standard constructor for OMNeT++ Define_Module()
         *
         * The actual parameters are loaded in the initialize() method!
         */
        OMNeTCubicEvaporationPolicy() : CubicEvaporationPolicy(0, 0, 3) {}

        virtual void initialize();
        virtual void handleMessage(cMessage* message);
};

OMNETARA_NAMESPACE_END

#endif 
