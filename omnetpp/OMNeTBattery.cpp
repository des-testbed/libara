/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTBattery.h"

#include <cstdio>

namespace ARA {
    namespace omnetpp {
        Define_Module(OMNeTBattery);

        void OMNeTBattery::initialize(int stage) {
            InetSimpleBattery::initialize(stage);
            if(stage == 0) {
                WATCH(residualCapacity);
            }
        }

        void OMNeTBattery::deductAndCheck(){
            if (residualCapacity <= 0) {
                // The battery is already depleted and devices should have stopped sending drawMsg. However leftover messages in queue are caught
                return;
            }

            calculateConsumedEnergy();
            evaluateCurrentEnergyLevel();
            recordStatistics();
        }

        void OMNeTBattery::calculateConsumedEnergy() {
            SimTime now = simTime();

            /**
             * If device[i] has never drawn current (e.g. because the device
             * hasn't been used yet or only uses ENERGY) the currentActivity is
             * still -1.  If the device is not drawing current at the moment,
             * draw has been reset to 0, so energy is also 0.  (It might perhaps
             * be wise to guard more carefully against fp issues later.)
             */
            for (unsigned int i = 0; i < deviceEntryVector.size(); i++) {
                int currentActivity = deviceEntryVector[i]->currentActivity;
                if (currentActivity > -1) {
                    double energy = deviceEntryVector[i]->draw * voltage * (now - lastUpdateTime).dbl();
                    if (energy > 0) {
                        deviceEntryVector[i]->accts[currentActivity] += energy;
                        deviceEntryVector[i]->times[currentActivity] += (now - lastUpdateTime);
                        residualCapacity -= energy;
                    }
                }
            }

            for (DeviceEntryMap::iterator it = deviceEntryMap.begin(); it!=deviceEntryMap.end(); it++) {
                int currentActivity = it->second->currentActivity;
                if (currentActivity > -1) {
                    double energy = it->second->draw * voltage * (now - lastUpdateTime).dbl();
                    if (energy > 0) {
                        it->second->accts[currentActivity] += energy;
                        it->second->times[currentActivity] += (now - lastUpdateTime);
                        residualCapacity -= energy;
                    }
                }
            }

            EV << "residual capacity = " << residualCapacity << "\n";
            lastUpdateTime = now;
        }

        void OMNeTBattery::evaluateCurrentEnergyLevel() {
            updateBatteryIcon();
            if (residualCapacity <= 0.0 ) {
                EV << "[BATTERY]: " << getParentModule()->getFullName() <<" 's battery exhausted" << "\n";
                publishEnergyInformation(0.0);
            }
            else {
                if (hasBatteryCapacityChangedEnough() == true) {
                    EV << "[BATTERY]: " << getParentModule()->getFullName() << " 's battery energy left: " << lastPublishCapacity  << "%" << "\n";
                    publishEnergyInformation(residualCapacity);
                }
            }
        }

        void OMNeTBattery::updateBatteryIcon() {
            char buffer[64];
            std::sprintf(buffer, "c: %.1f", residualCapacity);

            cDisplayString& displayString = getDisplayString();
            displayString.setTagArg("t", 0, buffer);
        }

        void OMNeTBattery::publishEnergyInformation(double publishedEnergyLevel) {
            Energy* energyInformation = new Energy(publishedEnergyLevel);
            mpNb->fireChangeNotification(NF_BATTERY_CHANGED, energyInformation);
            delete energyInformation;

            lastPublishCapacity = publishedEnergyLevel;
        }

        bool OMNeTBattery::hasBatteryCapacityChangedEnough() {
            return (lastPublishCapacity - residualCapacity) / capacity >= publishDelta;
        }

        void OMNeTBattery::recordStatistics() {
            residualVec.record(residualCapacity);

            if (mCurrEnergy) {
                mCurrEnergy->record(capacity-residualCapacity);
            }
        }
    }
}
