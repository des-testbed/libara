/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include "testAPI/mocks/LoggerMock.h"

using namespace ARA;
using namespace std;

TEST_GROUP(LoggerMockTest) {
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

TEST(LoggerMockTest, create) {
    BYTES_EQUAL(0, logger->getNrOfLoggedMessages());
    CHECK(logger->getLoggedMessages()->empty());
}

TEST(LoggerMockTest, logMessage) {
    logger->message("This is a test");
    CHECK(hasLoggedMessage("This is a test", Logger::LEVEL_INFO));
}
