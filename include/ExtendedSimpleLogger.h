/*
 * $FU-Copyright$
 */

#ifndef EXTENDED_SIMPLE_LOGGER_H_
#define EXTENDED_SIMPLE_LOGGER_H_

#include "ARAMacros.h"
#include "SimpleLogger.h"

#include <mutex> 

ARA_NAMESPACE_BEGIN

class ExtendedSimpleLogger : public SimpleLogger {
    public:
        ExtendedSimpleLogger() : SimpleLogger("") {};
        ExtendedSimpleLogger(const char* instanceName) : SimpleLogger(instanceName) {};

    protected:
        void performLoggingAction(const std::string &logMessage, Level level, va_list args) const;

    private:
        mutable std::mutex mutex;
};

ARA_NAMESPACE_END

#endif // EXTENDED_SIMPLE_LOGGER_H_
