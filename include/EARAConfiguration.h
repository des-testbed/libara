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
     * Returns the time in milliseconds that shall pass between the
     * periodic sending of the energy dissemination packets.
     */
    virtual unsigned int getEnergyDisseminationTimeout() const = 0;
};

ARA_NAMESPACE_END
#endif
