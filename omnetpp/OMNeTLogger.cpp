/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTLogger.h"

#include <cstdio>
#include <iostream>

using namespace std;

OMNETARA_NAMESPACE_BEGIN

void OMNeTLogger::performLoggingAction(const std::string &logMessage, Level level, va_list args) const {
    char buffer[512]; // FIXME a dynamic buffer size would be awesome

    SimTime currentSimulationTime = simTime();
    char timeString[14]; // Time format: 123.123456 + space + null byte
    sprintf(timeString, "[%10.6f] ", currentSimulationTime.dbl());

    vsprintf(buffer, logMessage.c_str(), args);
    cout << timeString << " " << instanceName << "  [" << getLevelString(level) << "] " << buffer << "\n";
}

OMNETARA_NAMESPACE_END
