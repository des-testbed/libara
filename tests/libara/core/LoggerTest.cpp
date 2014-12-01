/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/libara/LoggerMock.h"

#include <string>

using namespace ARA;
using namespace std;

TEST_GROUP(LoggerTest) {
    LoggerMock* logger;

    void setup() {
        logger = new LoggerMock();
    }

    void teardown() {
        delete logger;
    }

    /**
     * Checks if one of the logged messages equals the given text and log level
     */
    bool hasLoggedMessage(string message, int level) {
        deque<LogMessage>* loggedMessages = logger->getLoggedMessages();
        for(auto& loggedMessage: *loggedMessages) {
            if(loggedMessage.level == level && loggedMessage.text == message) {
                return true;
            }
        }

        return false;
    }

};

TEST(LoggerTest, logTrace) {
    string message = string("Hello trace world");
    logger->trace(message);
    CHECK(hasLoggedMessage(message, Logger::LEVEL_TRACE));
}

TEST(LoggerTest, logTraceWithArgs) {
    string message = string("Hello trace world %s");
    logger->trace(message, "Argument");
    CHECK(hasLoggedMessage("Hello trace world Argument", Logger::LEVEL_TRACE));
}

TEST(LoggerTest, logDebug) {
    string message = string("Hello debug world");
    logger->debug(message);
    CHECK(hasLoggedMessage(message, Logger::LEVEL_DEBUG));
}

TEST(LoggerTest, logDebugWithArgs) {
    string message = string("Hello debug world %s");
    logger->debug(message, "Argument");
    CHECK(hasLoggedMessage("Hello debug world Argument", Logger::LEVEL_DEBUG));
}

TEST(LoggerTest, logInfo) {
    string message = string("Hello info world");
    logger->info(message);
    CHECK(hasLoggedMessage(message, Logger::LEVEL_INFO));
}

TEST(LoggerTest, logInfoWithArgs) {
    string message = string("Hello info world %s");
    logger->info(message, "Argument");
    CHECK(hasLoggedMessage("Hello info world Argument", Logger::LEVEL_INFO));
}

TEST(LoggerTest, logWarn) {
    string message = string("Hello warn world");
    logger->warn(message);
    CHECK(hasLoggedMessage(message, Logger::LEVEL_WARN));
}

TEST(LoggerTest, logWarnWithArgs) {
    string message = string("Hello warn world %s");
    logger->warn(message, "Argument");
    CHECK(hasLoggedMessage("Hello warn world Argument", Logger::LEVEL_WARN));
}

TEST(LoggerTest, logError) {
    string message = string("Hello error world");
    logger->error(message);
    CHECK(hasLoggedMessage(message, Logger::LEVEL_ERROR));
}

TEST(LoggerTest, logErrorWithArgs) {
    string message = string("Hello error world %s");
    logger->error(message, "Argument");
    CHECK(hasLoggedMessage("Hello error world Argument", Logger::LEVEL_ERROR));
}

TEST(LoggerTest, logFatal) {
    string message = string("Hello fatal world");
    logger->fatal(message);
    CHECK(hasLoggedMessage(message, Logger::LEVEL_FATAL));
}

TEST(LoggerTest, logFatalWithArgs) {
    string message = string("Hello fatal world %s");
    logger->fatal(message, "Argument");
    CHECK(hasLoggedMessage("Hello fatal world Argument", Logger::LEVEL_FATAL));
}

TEST(LoggerTest, getLevelString) {
    STRCMP_EQUAL("TRACE", Logger::getLevelString(Logger::LEVEL_TRACE));
    STRCMP_EQUAL("DEBUG", Logger::getLevelString(Logger::LEVEL_DEBUG));
    STRCMP_EQUAL("INFO", Logger::getLevelString(Logger::LEVEL_INFO));
    STRCMP_EQUAL("WARN", Logger::getLevelString(Logger::LEVEL_WARN));
    STRCMP_EQUAL("ERROR", Logger::getLevelString(Logger::LEVEL_ERROR));
    STRCMP_EQUAL("FATAL", Logger::getLevelString(Logger::LEVEL_FATAL));
}
