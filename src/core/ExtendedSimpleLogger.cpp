/*
 * $FU-Copyright$
 */

#include "ExtendedSimpleLogger.h"

#include <cstdio>
#include <iostream>

using namespace std;

ARA_NAMESPACE_BEGIN

/**
 *
 */
void ExtendedSimpleLogger::performLoggingAction(const std::string &logMessage, Level level, va_list args) const {
    std::unique_lock<std::mutex> lock(mutex);
    SimpleLogger::performLoggingAction(logMessage, level, args);	
}

ARA_NAMESPACE_END

