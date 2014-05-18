/*
 * $FU-Copyright$
 */

#include "SimpleLoggerExtended.h"

#include <cstdio>
#include <iostream>

using namespace std;

ARA_NAMESPACE_BEGIN

/**
 *
 */
void SimpleLoggerExtended::performLoggingAction(const std::string &logMessage, Level level, va_list args) const {
    std::lock_guard<std::mutex> lock(mutex);
    SimpleLogger::performLoggingAction(logMessage, level, args);	
}

ARA_NAMESPACE_END

