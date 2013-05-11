/*
 * $FU-Copyright$
 */

#ifndef OMNET_LOGGER_H_
#define OMNET_LOGGER_H_

#include "OMNeTARAMacros.h"
#include "Logger.h"

OMNETARA_NAMESPACE_BEGIN

    class OMNeTLogger : public Logger {
    public:
        OMNeTLogger(const char* instanceName) : instanceName(instanceName) {};

    protected:
        void performLoggingAction(const std::string &logMessage, Level level, va_list args) const;

    private:
        const char* instanceName;
    };

OMNETARA_NAMESPACE_END

#endif
