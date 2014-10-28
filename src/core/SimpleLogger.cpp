/*
 * $FU-Copyright$
 */

#include "SimpleLogger.h"

namespace ARA {


// TODO: we should fix this since 'va_start' has undefined behavior with reference types
void SimpleLogger::performLoggingAction(const std::string &logMessage, Level level, va_list args) const {
    char buffer[512]; // FIXME a dynamic buffer size would be awesome
    vsprintf(buffer, logMessage.c_str(), args);
    std::cerr << instanceName << "  [" << getLevelString(level) << "] " << buffer << "\n";
}

} /* namespace ARA */
