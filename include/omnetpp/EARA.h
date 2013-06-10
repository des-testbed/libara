/*
 * $FU-Copyright$
 */

#ifndef OMNET_EARA_H_
#define OMNET_EARA_H_

#include "OMNeTARAMacros.h"

#include "AbstractEARAClient.h"
#include "AbstractOMNeTARAClient.h"
#include "Energy.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * TODO write class description
 */
class EARA: public AbstractEARAClient, public AbstractOMNeTARAClient {
    public:
        ~EARA();

        // some signals for statistics recording
        static simsignal_t LOOP_DETECTION_SIGNAL;
        static simsignal_t DROP_PACKET_WITH_ZERO_TTL;
        static simsignal_t ROUTE_FAILURE_NO_HOP;
        static simsignal_t NEW_ROUTE_DISCOVERY;
        static simsignal_t ROUTE_FAILURE_NEXT_HOP_IS_SENDER;
        static simsignal_t DROP_PACKET_BECAUSE_ENERGY_DEPLETED;

    protected:
        virtual int numInitStages() const;
        virtual void initialize(int stage);
        virtual void finish();

        virtual void receivePacket(Packet* packet, NetworkInterface* interface);

        virtual void handleMessage(cMessage* message);

        virtual void handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface);

        virtual void handlePacketWithZeroTTL(Packet* packet);

        virtual void handleNonSourceRouteDiscovery(Packet* packet);

        virtual void startNewRouteDiscovery(Packet* packet);

        virtual void receiveChangeNotification(int category, const cObject* details);

        virtual bool handleBrokenOMNeTLink(OMNeTPacket* packet, AddressPtr receiverAddress, NetworkInterface* interface);

        virtual unsigned char getCurrentEnergyLevel();
    private:
        void handleBatteryStatusChange(Energy* energyInformation);

    private:
        int nrOfDetectedLoops = 0;
        bool hasEnoughBattery = true;
        double maximumBatteryLevel;
        int currentEnergyLevel;

        cOutVector energyLevelOutVector;
        SimTime nodeEnergyDepletionTimestamp = -1;
};

OMNETARA_NAMESPACE_END

#endif /* OMNET_EARA_H_ */
