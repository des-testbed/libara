/*
 * $FU-Copyright$
 */

#include "LoggerMock.h"

using namespace ARA;

LoggerMock::LoggerMock() {
    loggedMessages = new std::deque<LogMessage>();
}

LoggerMock::~LoggerMock() {
    delete loggedMessages;
}

void LoggerMock::message(const std::string &logMessage, Level level) const {
    LogMessage newLogMessage;
    newLogMessage.text = logMessage;
    newLogMessage.level = level;

    loggedMessages->push_back(newLogMessage);
}

unsigned int LoggerMock::getNrOfLoggedMessages() const {
    return loggedMessages->size();
}

std::deque<LogMessage>* LoggerMock::getLoggedMessages() const {
    return loggedMessages;
}
