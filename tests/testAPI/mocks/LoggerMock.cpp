/*
 * $FU-Copyright$
 */

#include "LoggerMock.h"

#include <cstdio>

using namespace ARA;

LoggerMock::LoggerMock() {
    loggedMessages = new std::deque<LogMessage>();
}

LoggerMock::~LoggerMock() {
    delete loggedMessages;
}

void LoggerMock::performLoggingAction(const std::string &text, Level level, va_list args) const {
    char buffer[512]; // FIXME a dynamic buffer size would be awesome
    std::vsprintf(buffer, text.c_str(), args);
    LogMessage newLogMessage;
    newLogMessage.text = buffer;
    newLogMessage.level = level;

    loggedMessages->push_back(newLogMessage);
}

unsigned int LoggerMock::getNrOfLoggedMessages() const {
    return loggedMessages->size();
}

std::deque<LogMessage>* LoggerMock::getLoggedMessages() const {
    return loggedMessages;
}
