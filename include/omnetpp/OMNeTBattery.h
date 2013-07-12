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

/**
 * The class represents a simple implementation of a battery. It is
 * based on the battery interface as provided in the INETMANET package.
 */
class OMNeTBattery : public BasicBattery {

    static int NO_ACTIVITY;

    /**
     * Private class to hold all informations on registered devices
     */
    class DeviceEntry {
        public:
            int currentActivity;            // the current activity of a device (like sendig, receiving, idle, etc)
            double currentEnergyDraw;       // the amount of energy, the current activity consumes (in mA)
            double radioUsageCurrent[4];    // the amount of energy each wireless activity consumes when active
            cObject* owner;                 // is only used to check if a device has been registered before (not wireless)
            int numberOfActivities;         // is only used for checking if a radio state can be mapped to an activity
            double* accts;

            DeviceEntry(int nrOfActivities) {
                ASSERT(nrOfActivities > 0);

                numberOfActivities = nrOfActivities;
                currentActivity = NO_ACTIVITY;
                owner = NULL;
                accts = new double[nrOfActivities];
                for (int i=0; i<nrOfActivities; i++) {
                    radioUsageCurrent[i] = 0.0;
                    accts[i] = 0;
                }
            }

            ~DeviceEntry() {
                delete [] accts;
            }
    };

    typedef std::map<int,DeviceEntry*>  DeviceEntryMap;
    typedef std::vector<DeviceEntry*>  DeviceEntryVector;

    public:
        OMNeTBattery(){};
        ~OMNeTBattery();

        virtual void initialize(int stage);
        virtual void handleMessage(cMessage* message);
        virtual void finish();

        virtual void receiveChangeNotification(int category, const cObject* notificationDetails);
        virtual int registerDevice(cObject* deviceID, int numberOfActvities);
        virtual void registerWirelessDevice(int deviceID, double usageWhenIdleInMilliAmpere, double usageWhenReceivingInMilliAmpere, double usageWhenSendingInMilliAmpere, double usageWhenSleepingInMilliAmpere);
        virtual void draw(int deviceID, DrawAmount& amount, int activity);
        double getCapacity();

    protected:
        void updateResidualEnergy();

    private:
        void checkCompatibilityToRadioState();
        void checkIfDeviceHasBeenRegisteredBefore(cObject* device);
        void checkIfWirelessDeviceHasBeenRegisteredBefore(int deviceID);
        void calculateConsumedEnergy();
        void calculateConsumedEnergyOfStandardDevices();
        void calculateConsumedEnergyOfWirelessDevices();
        void updateBatteryIcon();
        void recordResidualEnergy();
        void publishEnergyInformation();

    private:
        NotificationBoard*  notificationBoard;
        DeviceEntryMap deviceEntryMap;
        DeviceEntryVector deviceEntryVector;

        SimTime updateInterval;
        SimTime lastUpdateTime;
        bool hasAlreadySubscribedToRadioStateChanged = false;
        double voltageInVolts;
        double capacityInMilliWattSeconds;
        double residualCapacityInMilliWattSeconds;
        cMessage* publishMessage;

        // For recording the energy values
        cOutVector residualEnergyOutVector;
        double energyVectorDelta;
        double lastStoredResidualEnergyInMilliWattSeconds;
};

OMNETARA_NAMESPACE_END

#endif

