/*
 * $FU-Copyright$
 */

#include "omnetpp/EARA.h"

OMNETARA_NAMESPACE_BEGIN

// Register the class with the OMNeT++ simulation
Define_Module(EARA);

void EARA::initialize(int stage) {
    ARA::initialize(stage);

    if(stage == 0) {
        NotificationBoard* notificationBoard = NotificationBoardAccess().get();
        notificationBoard->subscribe(this, NF_BATTERY_CHANGED);

        helloMessageTimeout = par("helloMessageTimeout");
        helloMessage = new cMessage("HELLO");
        scheduleAt(simTime() + helloMessageTimeout, helloMessage);

        WATCH(hasEnoughBattery);
    }
}

void EARA::handleMessage(cMessage* message) {
    if (hasEnoughBattery) {
        if(message == helloMessage) {
            //TODO
        }
        else {
            ARA::handleMessage(message);
        }
    }
}

void EARA::receiveChangeNotification(int category, const cObject* details) {
    if(category == NF_BATTERY_CHANGED) {
        handleBatteryStatusChange(check_and_cast<Energy*>(details));
    }
    else {
        ARA::receiveChangeNotification(category, details);
    }
}

void EARA::handleBatteryStatusChange(Energy* energyInformation) {
    if (energyInformation->GetEnergy() <= 0) {
       hasEnoughBattery = false;

       // change the node color
       cDisplayString& displayString = getParentModule()->getParentModule()->getDisplayString();
       displayString.setTagArg("i", 1, "#FF0000");
    }
}

OMNETARA_NAMESPACE_END
