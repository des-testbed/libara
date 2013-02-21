/*
 * $FU-Copyright$
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>

namespace ARA {

/**
 * TODO write class description.
 */
class Logger {
public:
    virtual ~Logger() {}

    enum LogLevelEnum {
        LEVEL_TRACE,
        LEVEL_DEBUG,
        LEVEL_INFO,
        LEVEL_WARN,
        LEVEL_ERROR,
    };

    virtual void message(const std::string &logMessage, int level = LogLevelEnum::LEVEL_INFO) const = 0;

    void trace(const std::string &logMessage) const {
        message(logMessage, LogLevelEnum::LEVEL_TRACE);
    }

    void debug(const std::string &logMessage) const {
        message(logMessage, LogLevelEnum::LEVEL_DEBUG);
    }

    void warn(const std::string &logMessage) const {
        message(logMessage, LogLevelEnum::LEVEL_WARN);
    }

    void error(const std::string &logMessage) const {
        message(logMessage, LogLevelEnum::LEVEL_ERROR);
    }
};

} /* namespace ARA */
#endif // LOGGER_H_
