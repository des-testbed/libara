/*
 * $FU-Copyright$
 */

#ifndef OMNET_EARA_H_
#define OMNET_EARA_H_

#include "OMNeTARAMacros.h"

#include "AbstractEARAClient.h"
#include "AbstractOMNeTARAClient.h"
#include "Energy.h"
#include "MessageDispatcher.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * TODO write class description
 */
class EARA: public AbstractEARAClient, public AbstractOMNeTARAClient {
    public:
        EARA();
        ~EARA();

    protected:
        virtual int numInitStages() const;
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage* message);
        virtual void deliverToSystem(const Packet* packet);
        virtual void packetNotDeliverable(const Packet* packet);
        virtual void receiveChangeNotification(int category, const cObject* details);
        virtual void handleBrokenOMNeTLink(OMNeTPacket* packet, AddressPtr receiverAddress);
        virtual unsigned char getCurrentEnergyLevel();
    private:
        void handleBatteryStatusChange(Energy* energyInformation);

    private:
        MessageDispatcher* messageDispatcher;
        bool hasEnoughBattery = true;
        double maximumBatteryLevel;
        int currentEnergyLevel;
};

OMNETARA_NAMESPACE_END

#endif /* OMNET_EARA_H_ */
