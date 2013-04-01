/*
 * $FU-Copyright$
 */

#ifndef OMNET_EARA_H_
#define OMNET_EARA_H_

#include "omnetpp/OMNeTARAMacros.h"
#include "omnetpp/ARA.h"

OMNETARA_NAMESPACE_BEGIN

class EARA: public ARA {
    public:
        void initialize(int stage);
        void handleMessage(cMessage* message);

    protected:
        void receiveChangeNotification(int category, const cObject* details);

    private:
        void handleBatteryStatusChange(Energy* energyInformation);

    private:
        bool hasEnoughBattery = true;
};

OMNETARA_NAMESPACE_END

#endif /* OMNET_EARA_H_ */
