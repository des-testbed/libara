/*
 * $FU-Copyright$
 */

#include "SimpleLogger.h"

#include <cstdio>
#include <iostream>

using namespace std;

namespace ARA {

void SimpleLogger::performLoggingAction(const std::string &logMessage, Level level, va_list args) const {
    char buffer[512]; // FIXME a dynamic buffer size would be awesome
    vsprintf(buffer, logMessage.c_str(), args);
    cout << "[" << getLevelString(level) << "] " << buffer << "\n";
}

} /* namespace ARA */
