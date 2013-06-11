/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTBattery.h"
#include "RadioState.h"
#include "Energy.h"

OMNETARA_NAMESPACE_BEGIN

Define_Module(OMNeTBattery);

void OMNeTBattery::initialize(int stage) {
    if(stage == 0) {
        notificationBoard = NotificationBoardAccess().get();

        voltageInVolts = par("voltage");
        double nominalCapacityInMilliAmperePerHour = par("nominal");
        nominalCapacityInMilliWattPerSecond = nominalCapacityInMilliAmperePerHour * 60 * 60 * voltageInVolts;

        double capacityInMilliAmperePerHour = par("capacity");
        capacityInMilliWattPerSecond = capacityInMilliAmperePerHour * 60 * 60 * voltageInVolts;

        residualCapacityInMilliWattPerSecond = capacityInMilliWattPerSecond;
        lastPublishedCapacity = residualCapacityInMilliWattPerSecond;

        publishTime = par("publishTime");
        if (publishTime > 0) {
            lastUpdateTime = simTime();
            publishMessage = new cMessage("Update energy");
            publishMessage->setSchedulingPriority(2000);
            scheduleAt(publishTime, publishMessage);
        }

        /* TODO check if all parameters make sense
        if (nominalCapmAh <= 0) {
            error("Invalid nominal capacity value");
        }*/

        checkCompatibilityToRadioState();

        residualEnergyOutVector.setName("Residual Energy");
        WATCH(residualCapacityInMilliWattPerSecond);
        WATCH(lastPublishedCapacity);
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
    ASSERT(numberOfActvities > 0);

    DeviceEntry* newDevice = new DeviceEntry();
    newDevice->owner = device;
    newDevice->numAccts = numberOfActvities;
    newDevice->accts = new double[numberOfActvities];
    newDevice->times = new simtime_t[numberOfActvities];

    for (int i = 0; i < numberOfActvities; i++) {
        newDevice->accts[i] = 0;
        newDevice->times[i] = 0;
    }

    EV<< "Registered new power consuming device"  << deviceEntryVector.size() << " with " << numberOfActvities << " activities" << endl;
    deviceEntryVector.push_back(newDevice);
    return deviceEntryVector.size()-1;
}

void OMNeTBattery::checkIfDeviceHasBeenRegisteredBefore(cObject* device) {
    int nrOfRegisteredDevices = deviceEntryVector.size();
    for (unsigned int i = 0; i < nrOfRegisteredDevices; i++) {
        if (deviceEntryVector[i]->owner == device) {
            error("Device %s has been already registered!", device->getFullName());
        }
    }
}

void OMNeTBattery::registerWirelessDevice(int deviceID, double usageWhenIdle, double usageWhenReceiving, double usageWhenSending, double usageWhenSleeping) {
    Enter_Method_Silent();
    checkIfWirelessDeviceHasBeenRegisteredBefore(deviceID);

    DeviceEntry* newDevice = new DeviceEntry();
    newDevice->numAccts = 4;
    newDevice->accts = new double[4];
    newDevice->times = new simtime_t[4];
    newDevice->radioUsageCurrent[RadioState::IDLE] = usageWhenIdle;
    newDevice->radioUsageCurrent[RadioState::RECV] = usageWhenReceiving;
    newDevice->radioUsageCurrent[RadioState::TRANSMIT] = usageWhenSending;
    newDevice->radioUsageCurrent[RadioState::SLEEP] = usageWhenSleeping;

    for (int i = 0; i < 4; i++) {
        newDevice->accts[i] = 0;
        newDevice->times[i] = 0;
    }

    deviceEntryMap.insert(std::pair<int,DeviceEntry*>(deviceID, newDevice));
    if (hasAlreadySubscribedToRadioStateChanged == false) {
        notificationBoard->subscribe(this, NF_RADIOSTATE_CHANGED);
        hasAlreadySubscribedToRadioStateChanged = false;
    }
}

void OMNeTBattery::checkIfWirelessDeviceHasBeenRegisteredBefore(int deviceID) {
    if (deviceEntryMap.find(deviceID) != deviceEntryMap.end()) {
        error("Device with id %u has been already registered!", deviceID);
    }
}

void OMNeTBattery::handleMessage(cMessage* message) {
    if (message->isSelfMessage() == false) {
        error("Can not handle any external messages");
    }

    deductAndCheck();
    scheduleAt(simTime() + publishTime, publishMessage);
}

void OMNeTBattery::deductAndCheck(){
    if (residualCapacityInMilliWattPerSecond <= 0) {
        // The battery is already depleted and devices should have stopped sending drawMsg. However leftover messages in queue are caught
        return;
    }

    calculateConsumedEnergy();
    publishEnergyInformation(residualCapacityInMilliWattPerSecond);

    updateBatteryIcon();
    residualEnergyOutVector.record(residualCapacityInMilliWattPerSecond);
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
            double energy = deviceEntryVector[i]->draw * voltageInVolts * (now - lastUpdateTime).dbl();
            if (energy > 0) {
                deviceEntryVector[i]->accts[currentActivity] += energy;
                deviceEntryVector[i]->times[currentActivity] += (now - lastUpdateTime);
                residualCapacityInMilliWattPerSecond -= energy;
            }
        }
    }

    for (DeviceEntryMap::iterator it = deviceEntryMap.begin(); it!=deviceEntryMap.end(); it++) {
        int currentActivity = it->second->currentActivity;
        if (currentActivity > -1) {
            double energy = it->second->draw * voltageInVolts * (now - lastUpdateTime).dbl();
            if (energy > 0) {
                it->second->accts[currentActivity] += energy;
                it->second->times[currentActivity] += (now - lastUpdateTime);
                residualCapacityInMilliWattPerSecond -= energy;
            }
        }
    }

    if (residualCapacityInMilliWattPerSecond <= 0.0 ) {
        EV << "[BATTERY]: " << getParentModule()->getFullName() <<" 's battery exhausted" << "\n";
        residualCapacityInMilliWattPerSecond = 0;
    }
    else {
        EV << "[BATTERY]: residual capacity = " << residualCapacityInMilliWattPerSecond << "\n";
    }
    lastUpdateTime = now;
}

void OMNeTBattery::updateBatteryIcon() {
    char buffer[64];
    std::sprintf(buffer, "c: %.1f", residualCapacityInMilliWattPerSecond);

    cDisplayString& displayString = getDisplayString();
    displayString.setTagArg("t", 0, buffer);
}

void OMNeTBattery::publishEnergyInformation(double publishedEnergyLevel) {
    Energy* energyInformation = new Energy(publishedEnergyLevel);
    notificationBoard->fireChangeNotification(NF_BATTERY_CHANGED, energyInformation);
    delete energyInformation;

    lastPublishedCapacity = publishedEnergyLevel;
}

double OMNeTBattery::getNominalValue() {
    return nominalCapacityInMilliWattPerSecond;
}

void OMNeTBattery::receiveChangeNotification(int category, const cObject* notificationDetails) {
    Enter_Method_Silent();
    if (category == NF_RADIOSTATE_CHANGED)     {
        const RadioState* radioState = check_and_cast<const RadioState*>(notificationDetails);
        DeviceEntryMap::iterator foundDeviceEntry = deviceEntryMap.find(radioState->getRadioId());
        if (foundDeviceEntry==deviceEntryMap.end()) {
            return;
        }

        DeviceEntry* deviceEntry = foundDeviceEntry->second;
        if (radioState->getState() >= deviceEntry->numAccts) {
            error("Can not handle change in radio state (unkown state)");
        }

        double current = deviceEntry->radioUsageCurrent[radioState->getState()];

        EV << simTime() << " Wireless device " << radioState->getRadioId() << " drew current " << current << "mA, new state = " << radioState->getState() << std::endl;

        // update the residual capacity (finish previous current draw)
        deductAndCheck();

        // set the new current draw in the device vector
        deviceEntry->draw = current;
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
        deductAndCheck();

        // set the new current draw in the device vector
        deviceEntryVector[deviceID]->draw = current;
        deviceEntryVector[deviceID]->currentActivity = activity;
    }
    else if (amount.getType() == DrawAmount::ENERGY) {
        double energy = amount.getValue();
        if (!(activity >= 0 && activity < deviceEntryVector[deviceID]->numAccts)) {
            error("Invalid activity specified");
        }

        EV << simTime() << " device " << deviceID <<  " deduct " << energy << " mW-s, activity = " << activity << endl;

        // deduct a fixed energy cost
        deviceEntryVector[deviceID]->accts[activity] += energy;
        residualCapacityInMilliWattPerSecond -= energy;

        // update the residual capacity (ongoing current draw)
        deductAndCheck();
    }
    else {
        error("Unknown power type!");
    }
}

void OMNeTBattery::finish() {
    // do a final update of battery capacity
    deductAndCheck();
    deviceEntryMap.clear();
    deviceEntryVector.clear();
}

OMNETARA_NAMESPACE_END
