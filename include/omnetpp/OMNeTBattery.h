/*
 * $FU-Copyright$
 */

#ifndef OMNET_BATTERY_H_
#define OMNET_BATTERY_H_

#include "OMNeTARAMacros.h"
#include "BasicBattery.h"
#include "NotificationBoard.h"

#include <map>
#include <vector>

OMNETARA_NAMESPACE_BEGIN

class DeviceEntry {
    public:
        int currentState;
        cObject* owner;
        double radioUsageCurrent[4];
        double draw;
        int currentActivity;
        int numAccts;
        double* accts;
        SimTime* times;

        DeviceEntry() {
            currentState = 0;
            numAccts = 0;
            currentActivity = -1;
            accts = NULL;
            times = NULL;
            owner = NULL;
            for (int i=0; i<4; i++) {
                radioUsageCurrent[i] = 0.0;
            }
        }

        ~DeviceEntry() {
            delete [] accts;
            delete [] times;
        }
};

typedef std::map<int,DeviceEntry*>  DeviceEntryMap;
typedef std::vector<DeviceEntry*>  DeviceEntryVector;

/**
 * The class represents a simple implementation of a battery. It is
 * based on the battery interface as provided in the INETMANET package.
 */
class OMNeTBattery : public BasicBattery {
    public:
        OMNeTBattery(){};
        ~OMNeTBattery();

        virtual void initialize(int stage);
        virtual void handleMessage(cMessage* message);
        virtual void finish();

        virtual void receiveChangeNotification(int category, const cObject* notificationDetails);
        virtual int registerDevice(cObject* deviceID, int numberOfActvities);
        virtual void registerWirelessDevice(int deviceID, double usageWhenIdle, double usageWhenReceiving, double usageWhenSending, double usageWhenSleeping);
        virtual void draw(int deviceID, DrawAmount& amount, int activity);
        double getNominalValue();

    protected:
        void deductAndCheck();

    private:
        void checkCompatibilityToRadioState();
        void checkIfDeviceHasBeenRegisteredBefore(cObject* device);
        void checkIfWirelessDeviceHasBeenRegisteredBefore(int deviceID);
        void calculateConsumedEnergy();
        void updateBatteryIcon();
        void publishEnergyInformation(double publishedEnergyLevel);

    private:
        NotificationBoard*  notificationBoard;
        DeviceEntryMap deviceEntryMap;
        DeviceEntryVector deviceEntryVector;

        SimTime publishTime;
        SimTime lastUpdateTime;
        bool hasAlreadySubscribedToRadioStateChanged = false;
        double voltageInVolts;
        double nominalCapacityInMilliWattPerSecond;
        double capacityInMilliWattPerSecond;
        double residualCapacityInMilliWattPerSecond;
        double lastPublishedCapacity;
        cMessage* publishMessage;

        cOutVector residualEnergyOutVector;
};

OMNETARA_NAMESPACE_END

#endif

