/*
 * $FU-Copyright$
 */

#ifndef SIMPLELOGGER_H_
#define SIMPLELOGGER_H_

#include "Logger.h"

namespace ARA {

    class SimpleLogger : public Logger {
    protected:
        void performLoggingAction(const std::string &logMessage, Level level, va_list args) const;
    };

} /* namespace ARA */
#endif // SIMPLELOGGER_H_
