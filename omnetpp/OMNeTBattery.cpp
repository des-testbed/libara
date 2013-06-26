/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTBattery.h"
#include "RadioState.h"
#include "Energy.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(OMNeTBattery);

int OMNeTBattery::NO_ACTIVITY = -1;

void OMNeTBattery::initialize(int stage) {
    if(stage == 0) {
        notificationBoard = NotificationBoardAccess().get();
        checkCompatibilityToRadioState();

        voltageInVolts = par("voltage");
        double capacityInMilliAmpereHours = par("capacity");
        capacityInMilliWattSeconds = capacityInMilliAmpereHours * 60 * 60 * voltageInVolts;
        residualCapacityInMilliWattSeconds = capacityInMilliWattSeconds;

        updateInterval = par("updateInterval");
        if (updateInterval > 0) {
            lastUpdateTime = simTime();
            publishMessage = new cMessage("Update energy");
            publishMessage->setSchedulingPriority(2000);
            scheduleAt(updateInterval, publishMessage);
        }
        else {
            EV << "Warning: Battery is disabled (updateInterval is <= 0)" << std::endl;
        }

        residualEnergyOutVector.setName("Residual Energy");
        WATCH(residualCapacityInMilliWattSeconds);
        updateBatteryIcon();
    }
}

OMNeTBattery::~OMNeTBattery() {
    while (deviceEntryMap.empty() == false) {
        delete deviceEntryMap.begin()->second;
        deviceEntryMap.erase(deviceEntryMap.begin());
    }

    while (!deviceEntryVector.empty()) {
        delete deviceEntryVector.back();
        deviceEntryVector.pop_back();
    }

    if (publishMessage) {
        cancelAndDelete(publishMessage);
    }
}

void OMNeTBattery::checkCompatibilityToRadioState() {
    if (RadioState::IDLE >= 4 || RadioState::RECV >= 4 || RadioState::TRANSMIT >= 4 || RadioState::SLEEP >= 4) {
        error("OMneTBattery is not compatible with RadioState (more than 4 states)");
    }
}

int OMNeTBattery::registerDevice(cObject* device, int numberOfActvities) {
    checkIfDeviceHasBeenRegisteredBefore(device);

    DeviceEntry* newDevice = new DeviceEntry(numberOfActvities);
    newDevice->owner = device;

    EV<< "Registered new power consuming device"  << deviceEntryVector.size() << " with " << numberOfActvities << " activities" << endl;
    deviceEntryVector.push_back(newDevice);

    int idOfRegisteredDevice = deviceEntryVector.size()-1;
    return idOfRegisteredDevice;
}

void OMNeTBattery::checkIfDeviceHasBeenRegisteredBefore(cObject* device) {
    int nrOfRegisteredDevices = deviceEntryVector.size();
    for (unsigned int i = 0; i < nrOfRegisteredDevices; i++) {
        if (deviceEntryVector[i]->owner == device) {
            error("Device %s has been already registered!", device->getFullName());
        }
    }
}

void OMNeTBattery::registerWirelessDevice(int deviceID, double usageWhenIdleInMilliAmpere, double usageWhenReceivingInMilliAmpere, double usageWhenSendingInMilliAmpere, double usageWhenSleepingInMilliAmpere) {
    Enter_Method_Silent();
    checkIfWirelessDeviceHasBeenRegisteredBefore(deviceID);

    int nrofActivities = 4;
    DeviceEntry* newDevice = new DeviceEntry(nrofActivities);
    newDevice->radioUsageCurrent[RadioState::IDLE] = usageWhenIdleInMilliAmpere;
    newDevice->radioUsageCurrent[RadioState::RECV] = usageWhenReceivingInMilliAmpere;
    newDevice->radioUsageCurrent[RadioState::TRANSMIT] = usageWhenSendingInMilliAmpere;
    newDevice->radioUsageCurrent[RadioState::SLEEP] = usageWhenSleepingInMilliAmpere;

    deviceEntryMap.insert(std::pair<int,DeviceEntry*>(deviceID, newDevice));

    if (hasAlreadySubscribedToRadioStateChanged == false) {
        notificationBoard->subscribe(this, NF_RADIOSTATE_CHANGED);
        hasAlreadySubscribedToRadioStateChanged = true;
    }
}

void OMNeTBattery::checkIfWirelessDeviceHasBeenRegisteredBefore(int deviceID) {
    if (deviceEntryMap.find(deviceID) != deviceEntryMap.end()) {
        error("Wireless device with id %u has already been registered!", deviceID);
    }
}

void OMNeTBattery::handleMessage(cMessage* message) {
    if (message->isSelfMessage() == false) {
        error("Can not handle any external messages");
    }

    updateResidualEnergy();
    scheduleAt(simTime() + updateInterval, publishMessage);
}

void OMNeTBattery::updateResidualEnergy(){
    if (residualCapacityInMilliWattSeconds <= 0) {
        // The battery is already depleted and devices should have stopped sending drawMsg. However leftover messages in queue are caught
        return;
    }

    calculateConsumedEnergy();
    publishEnergyInformation(residualCapacityInMilliWattSeconds);
    updateBatteryIcon();
    residualEnergyOutVector.record(residualCapacityInMilliWattSeconds);
}

void OMNeTBattery::calculateConsumedEnergy() {
    calculateConsumedEnergyOfStandardDevices();
    calculateConsumedEnergyOfWirelessDevices();

    if (residualCapacityInMilliWattSeconds <= 0.0 ) {
        EV << "[BATTERY]: " << getParentModule()->getFullName() <<" 's battery exhausted" << "\n";
        residualCapacityInMilliWattSeconds = 0;
    }
    else {
        EV << "[BATTERY]: residual capacity = " << residualCapacityInMilliWattSeconds << "\n";
    }

    lastUpdateTime = simTime();
}

void OMNeTBattery::calculateConsumedEnergyOfStandardDevices() {
    SimTime now = simTime();
    for (unsigned int i = 0; i < deviceEntryVector.size(); i++) {
        int currentActivity = deviceEntryVector[i]->currentActivity;
        if (currentActivity != NO_ACTIVITY) {
            double usedEnergy = deviceEntryVector[i]->currentEnergyDraw * voltageInVolts * (now - lastUpdateTime).dbl();
            if (usedEnergy > 0) {
                deviceEntryVector[i]->accts[currentActivity] += usedEnergy;
                residualCapacityInMilliWattSeconds -= usedEnergy;
            }
        }
    }
}

void OMNeTBattery::calculateConsumedEnergyOfWirelessDevices() {
    SimTime now = simTime();
    for (DeviceEntryMap::iterator it = deviceEntryMap.begin(); it!=deviceEntryMap.end(); it++) {
        DeviceEntry* deviceEntry = it->second;
        int currentActivity = deviceEntry->currentActivity;
        if (currentActivity != NO_ACTIVITY) {
            double usedEnergy = deviceEntry->currentEnergyDraw * voltageInVolts * (now - lastUpdateTime).dbl();
            if (usedEnergy > 0) {
                deviceEntry->accts[currentActivity] += usedEnergy;
                residualCapacityInMilliWattSeconds -= usedEnergy;
            }
        }
    }
}

void OMNeTBattery::updateBatteryIcon() {
    char buffer[64];
    std::sprintf(buffer, "c: %.1f", residualCapacityInMilliWattSeconds);

    cDisplayString& displayString = getDisplayString();
    displayString.setTagArg("t", 0, buffer);
}

void OMNeTBattery::publishEnergyInformation(double publishedEnergyLevel) {
    Energy* energyInformation = new Energy(publishedEnergyLevel);
    notificationBoard->fireChangeNotification(NF_BATTERY_CHANGED, energyInformation);
    delete energyInformation;
}

double OMNeTBattery::getCapacity() {
    return capacityInMilliWattSeconds;
}

void OMNeTBattery::receiveChangeNotification(int category, const cObject* notificationDetails) {
    Enter_Method_Silent();
    if (category == NF_RADIOSTATE_CHANGED)     {
        const RadioState* radioState = check_and_cast<const RadioState*>(notificationDetails);
        DeviceEntryMap::iterator foundDeviceEntry = deviceEntryMap.find(radioState->getRadioId());
        if (foundDeviceEntry==deviceEntryMap.end()) {
            // device has not been registered
            return;
        }

        DeviceEntry* deviceEntry = foundDeviceEntry->second;
        if (radioState->getState() >= deviceEntry->numberOfActivities) {
            error("Can not handle change in radio state (unkown state)");
        }

        double current = deviceEntry->radioUsageCurrent[radioState->getState()];

        EV << simTime() << " Wireless device " << radioState->getRadioId() << " drew current " << current << "mA, new state = " << radioState->getState() << std::endl;

        // update the residual capacity (finish previous current draw)
        updateResidualEnergy();

        // set the new current draw in the device vector
        deviceEntry->currentEnergyDraw = current;
        deviceEntry->currentActivity = radioState->getState();
    }
}

void OMNeTBattery::draw(int deviceID, DrawAmount& amount, int activity) {
    if (amount.getType() == DrawAmount::CURRENT) {
        double current = amount.getValue();
        if (activity < 0 && current != 0) {
            error("Invalid parameters for OMNeTBattery::draw");
        }

        EV << simTime() << " device " << deviceID << " drew current " << current << "mA, activity = " << activity << endl;

        // update the residual capacity (finish previous current draw)
        updateResidualEnergy();

        // set the new current draw in the device vector
        deviceEntryVector[deviceID]->currentEnergyDraw = current;
        deviceEntryVector[deviceID]->currentActivity = activity;
    }
    else if (amount.getType() == DrawAmount::ENERGY) {
        double energy = amount.getValue();
        if (!(activity >= 0 && activity < deviceEntryVector[deviceID]->numberOfActivities)) {
            error("Invalid activity specified");
        }

        EV << simTime() << " device " << deviceID <<  " deduct " << energy << " mW-s, activity = " << activity << endl;

        // deduct a fixed energy cost
        deviceEntryVector[deviceID]->accts[activity] += energy;
        residualCapacityInMilliWattSeconds -= energy;

        // update the residual capacity (ongoing current draw)
        updateResidualEnergy();
    }
    else {
        error("Unknown power type!");
    }
}

void OMNeTBattery::finish() {
    // do a final update of battery capacity
    updateResidualEnergy();
    deviceEntryMap.clear();
    deviceEntryVector.clear();
}

OMNETARA_NAMESPACE_END
