/*
 * $FU-Copyright$
 */

#ifndef SIMPLE_LOGGER_EXTENDED_H_
#define SIMPLE_LOGGER_EXTENDED_H_

#include "ARAMacros.h"
#include "SimpleLogger.h"

#include <mutex> 

ARA_NAMESPACE_BEGIN

class SimpleLoggerExtended : public SimpleLogger {
    public:
        SimpleLoggerExtended() : SimpleLogger("") {};
        SimpleLoggerExtended(const char* instanceName) : SimpleLogger(instanceName) {};

    protected:
        void performLoggingAction(const std::string &logMessage, Level level, va_list args) const;

    private:
        mutable std::mutex mutex;
};

ARA_NAMESPACE_END

#endif 
