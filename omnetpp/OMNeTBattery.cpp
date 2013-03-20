#include "omnetpp/OMNeTBattery.h"

namespace ARA {
    namespace omnetpp {
        Define_Module(OMNeTBattery);

        void OMNeTBattery::deductAndCheck(){
            /**
             * The battery is already depleted and devices should have stopped sending drawMsg.
             * However leftover messages in queue are caught.
             */
            if (residualCapacity <= 0) {
                return;
            }

            /// get the current simulation time
            simtime_t now = simTime();

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


            lastUpdateTime = now;

            EV << "residual capacity = " << residualCapacity << "\n";
            cDisplayString* display_string = &getParentModule()->getDisplayString();

            /// battery is depleted
            if (residualCapacity <= 0.0 ){
                EV << "[BATTERY]: " << getParentModule()->getFullName() <<" 's battery exhausted" << "\n";
            //    display_string->setTagArg("i", 1, "#ff0000");
                Energy* p_ene = new Energy(0.0);
                /// we inform via the notification board that the battery is depleted
                mpNb->fireChangeNotification(NF_BATTERY_CHANGED, p_ene);
                delete p_ene;
            /// battery is not depleted, continue
            } else {
                /// publish the battery capacity if it changed by more than delta
                if ((lastPublishCapacity - residualCapacity)/capacity >= publishDelta) {
                    lastPublishCapacity = residualCapacity;
                    Energy* p_ene = new Energy(residualCapacity);
                    mpNb->fireChangeNotification(NF_BATTERY_CHANGED, p_ene);
                    delete p_ene;

                    display_string->setTagArg("i", 1, "#000000"); // black coloring
                    EV << "[BATTERY]: " << getParentModule()->getFullName() << " 's battery energy left: " << lastPublishCapacity  << "%" << "\n";
                }
            }

            residualVec.record(residualCapacity);

            if (mCurrEnergy) {
                mCurrEnergy->record(capacity-residualCapacity);
            }
        } 
    }
}
