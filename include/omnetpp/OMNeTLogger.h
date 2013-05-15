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
        OMNeTLogger(const char* instanceName, Level logLevel=Level::LEVEL_WARN) : currentlogLevel(logLevel), instanceName(instanceName) {};
        void setLogLevel(Level newLevel);

    protected:
        void performLoggingAction(const std::string &logMessage, Level level, va_list args) const;

    private:
        Level currentlogLevel;
        const char* instanceName;
    };

OMNETARA_NAMESPACE_END

#endif
