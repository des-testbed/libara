/*
 * $FU-Copyright$
 */

#ifndef OMNET_EARA_CONFIGURATION_H_
#define OMNET_EARA_CONFIGURATION_H_

#include "OMNeTARAMacros.h"
#include "OMNeTConfiguration.h"
#include "EARAConfiguration.h"
#include "EnergyAwareRoutingTable.h"

OMNETARA_NAMESPACE_BEGIN

class OMNeTEARAConfiguration : public virtual OMNeTConfiguration, public EARAConfiguration {
    public:
        OMNeTEARAConfiguration(cModule* module);

        virtual float getInfluenceOfMinimumEnergyValue() const;
        virtual EnergyAwareRoutingTable* getRoutingTable();

    private:
        float influenceOfMinimumEnergyValue;
};

OMNETARA_NAMESPACE_END
#endif
