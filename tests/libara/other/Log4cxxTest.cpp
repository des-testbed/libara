/*
 * $FU-Copyright$
 */

#include "CppUTest/TestHarness.h"
#include <log4cxx/logger.h>


TEST_GROUP(Log4CxxTest) {
    log4cxx::LoggerPtr logger = log4cxx::LoggerPtr(log4cxx::Logger::getLogger("com.foo"));
};

IGNORE_TEST(Log4CxxTest, testUsage) {
    // TODO check log4cxx for the memory leak that is detected in this test
    LOG4CXX_INFO(logger, "Simple message text.")

    const char* region = "World";
    LOG4CXX_INFO(logger, "Hello, " << region)
}
