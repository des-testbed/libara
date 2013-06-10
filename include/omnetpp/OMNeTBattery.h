/*
 * $FU-Copyright$
 */

#ifndef OMNET_BATTERY_H_
#define OMNET_BATTERY_H_

#include "OMNeTARAMacros.h"
#include "Energy.h"
#include "InetSimpleBattery.h"

#include <algorithm>

OMNETARA_NAMESPACE_BEGIN

/**
 * The class represents a simple implementation of a battery. It is
 * based on the battery interface as provided in the INETMANET package.
 */
class OMNeTBattery : public InetSimpleBattery {
    public:
        OMNeTBattery(){};
        virtual ~OMNeTBattery(){};

        void initialize(int stage);
        double getNominalValue();

    protected:
        void deductAndCheck();

    private:
        void calculateConsumedEnergy();
        void evaluateCurrentEnergyLevel();
        void updateBatteryIcon();
        void publishEnergyInformation(double publishedEnergyLevel);
        bool hasBatteryCapacityChangedEnough();
        void recordStatistics();
};

OMNETARA_NAMESPACE_END

#endif

