/*
 * $FU-Copyright$
 */

#ifndef SIMPLELOGGER_H_
#define SIMPLELOGGER_H_

#include "Logger.h"

namespace ARA {

    class SimpleLogger : public Logger {
    public:
        SimpleLogger() : instanceName("") {};
        SimpleLogger(const char* instanceName) : instanceName(instanceName) {};

    protected:
        virtual void performLoggingAction(const std::string &logMessage, Level level, va_list args) const;

    private:
        const char* instanceName;
    };

} /* namespace ARA */
#endif // SIMPLELOGGER_H_
