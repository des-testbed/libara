/*
 * $FU-Copyright$
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <cstdarg>

namespace ARA {

/**
 * A logger is used to record or print out information about events that
 * may be noteworthy for the user or for debugging. This is an abstract
 * interface so we can limit the dependency of an external logging framework.
 *
 * The user can control the amount of information that is logged via the log level.
 * Currently there are the following five planned levels ordered by their severity:
 *     TRACE, DEBUG, INFO, WARN, ERROR and FATAL
 *
 * Implementation notes:
 * - Every concrete implementation of the Logger interface needs to implement the pure
 *   virtual protected method Logger::performLoggingAction(..)
 * - To speed up logging you should prefer passing the parameter for any formated string
 *   via the available varargs (like in printf). This should be faster than building all
 *   log strings even if they are not logged due to a too low log level.
 */
class Logger {
public:
    virtual ~Logger() {}

    enum Level {
        LEVEL_TRACE,
        LEVEL_DEBUG,
        LEVEL_INFO,
        LEVEL_WARN,
        LEVEL_ERROR,
        LEVEL_FATAL
    };

    /**
     * Performs logging for a message that is only of importance for very fine
     * grained debugging and development. Please use with caution so tracelogs
     * remain usable for everyone.
     */
    void trace(const std::string &logMessage, ...) const {
        va_list args;
        va_start(args, logMessage);
        performLoggingAction(logMessage, Level::LEVEL_TRACE, args);
    }

    /**
     * Performs logging for standard debugging messages. This level is mostly
     * used to debug abnormal behavior of the application.
     */
    void debug(const std::string &logMessage, ...) const {
        va_list args;
        va_start(args, logMessage);
        performLoggingAction(logMessage, Level::LEVEL_DEBUG, args);
    }

    /**
     * Performs logging for messages that highlight the progress of the
     * application at coarse-grained level.
     */
    void info(const std::string &logMessage, ...) const {
        va_list args;
        va_start(args, logMessage);
        performLoggingAction(logMessage, Level::LEVEL_INFO, args);
    }

    /**
     * Performs logging for messages that indicate potentially harmful
     * situations the user should now about.
     */
    void warn(const std::string &logMessage, ...) const {
        va_list args;
        va_start(args, logMessage);
        performLoggingAction(logMessage, Level::LEVEL_WARN, args);
    }

    /**
     * Performs logging for error messages that might still allow the
     * application to continue running but the user should be notified about.
     */
    void error(const std::string &logMessage, ...) const {
        va_list args;
        va_start(args, logMessage);
        performLoggingAction(logMessage, Level::LEVEL_ERROR, args);
    }

    /**
    * Performs logging for very severe error events that will presumably
    * lead the application to abort.
    */
   void fatal(const std::string &logMessage, ...) const {
       va_list args;
       va_start(args, logMessage);
       performLoggingAction(logMessage, Level::LEVEL_FATAL, args);
   }

   /**
    * Performs standard logging like the other available methods.
    * The only difference is that the third parameter if of the type
    * va_list to make it possible to call this method from another
    * method that accepts varargs with the three dots notation.
    *
    * Example:
    * <code>
    * class foo {
    *   doMyOwnLogging(char* text, ...) {
    *       va_list args;
    *       va_start(args, text);
    *       this->logger.logMessageWithVAList(text, Logger:LEVEL_INFO, args);
    *   }
    * }
    * </code>
    */
    void logMessageWithVAList(const std::string &logMessage, Level level, va_list args) const {
        performLoggingAction(logMessage, level, args);
    }

    static const char* getLevelString(Level level) {
        switch (level) {
            case Level::LEVEL_TRACE: return "TRACE";
            case Level::LEVEL_DEBUG: return "DEBUG";
            case Level::LEVEL_INFO:  return "INFO";
            case Level::LEVEL_WARN:  return "WARN";
            case Level::LEVEL_ERROR: return "ERROR";
            case Level::LEVEL_FATAL: return "FATAL";
            default: return "UNKOWN";
        }
    }

protected:

    /**
     * This method needs to be implemented by all concrete loggers.
     * It can never be called from the outside of this class.
     * Instead the convenience methods Logger::trace(..) Logger::debug(..)
     * Logger::info(..) Logger::warn(..) and Logger::error(..) are used.
     */
    virtual void performLoggingAction(const std::string &logMessage, Level level, va_list args) const = 0;

};

} /* namespace ARA */
#endif // LOGGER_H_
