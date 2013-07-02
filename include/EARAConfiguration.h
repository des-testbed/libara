/*
 * $FU-Copyright$
 */

#ifndef EARA_CONFIGURATION_H_
#define EARA_CONFIGURATION_H_

#include "ARAMacros.h"
#include "Configuration.h"

ARA_NAMESPACE_BEGIN

/**
 * This interface extends the ~Configuration interface and is used to retrieve the EARA specific configuration parameters.
 * A default implementation is available in ~BasicEARAConfiguration
 */
class EARAConfiguration : public virtual Configuration {
public:
    virtual ~EARAConfiguration() {};

    /**
     * Returns a value which indicates the impact of the Minimum energy value when the
     * route energy fitness is initialized.  The value needs to be >= 1.
     * Any value 1 <= b < 2 will favor the Minimum value over the Average.
     * Everything > 2 will put more focus on the Average value. See the Master's Thesis of
     * Friedrich Große for further explanation.
     */
    virtual float getInfluenceOfMinimumEnergyValue() const = 0;
};

ARA_NAMESPACE_END
#endif
