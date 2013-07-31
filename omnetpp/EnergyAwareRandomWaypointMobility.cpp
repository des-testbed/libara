/*
 * $FU-Copyright$
 */

#include "omnetpp/EnergyAwareRandomWaypointMobility.h"
#include "NotificationBoard.h"
#include "Energy.h"

OMNETARA_NAMESPACE_BEGIN

// Register the class with the OMNeT++ simulation
Define_Module(EnergyAwareRandomWaypointMobility);

void EnergyAwareRandomWaypointMobility::initialize(int stage) {
    RandomWPMobility::initialize(stage);
    if (stage == 0) {
        NotificationBoard* notificationBoard = NotificationBoardAccess().get();
        notificationBoard->subscribe(this, NF_BATTERY_CHANGED);
        isEnergyDepleted = false;
    }
}

void EnergyAwareRandomWaypointMobility::move() {
    if (isEnergyDepleted == false) {
        RandomWPMobility::move();
    }
    else {
        stationary = true;
        nextChange = -1;
    }
}

void EnergyAwareRandomWaypointMobility::receiveChangeNotification(int category, const cObject* details) {
    if(category == NF_BATTERY_CHANGED) {
        Energy* energyInformation = check_and_cast<Energy*>(details);
        if (energyInformation->GetEnergy() <= 0) {
            EV << "EnergyAwareRandomWaypointMobility detected that the battery is depleted and will stop moving after next target position has been reached" << std::endl;
            isEnergyDepleted = true;
        }
    }
}

OMNETARA_NAMESPACE_END
