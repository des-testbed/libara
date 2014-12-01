/*
 * $FU-Copyright$
 */

#ifndef LOGGERMOCK_H_
#define LOGGERMOCK_H_

#include "Logger.h"

#include <deque>
#include <string>

namespace ARA {

    struct LogMessage {
        Logger::Level level;
        std::string text;
    };

    class LoggerMock : public Logger {
    public:
        LoggerMock();
        ~LoggerMock();

        void performLoggingAction(const std::string &logMessage, Level level, va_list args) const;
        unsigned int getNrOfLoggedMessages() const;
        std::deque<LogMessage>* getLoggedMessages() const;

    private:
        std::deque<LogMessage>* loggedMessages;
    };

} /* namespace ARA */
#endif // LOGGERMOCK_H_
