/*
 * $FU-Copyright$
 */

#ifndef OMNETADDRESS_H_
#define OMNETADDRESS_H_

#include "OMNeTARAMacros.h"
#include "RandomWPMobility.h"
#include "INotifiable.h"

OMNETARA_NAMESPACE_BEGIN

class EnergyAwareRandomWaypointMobility : public RandomWPMobility {
    public:
        virtual void receiveChangeNotification(int category, const cObject* details);

    protected:
        virtual void initialize(int stage);
        virtual void move();

    private:
        bool isEnergyDepleted;
};

OMNETARA_NAMESPACE_END

#endif
