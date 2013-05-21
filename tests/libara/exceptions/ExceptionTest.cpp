/*
 * $FU-Copyright$
 */


#include "CppUTest/TestHarness.h"
#include "Exception.h"

using namespace ARA;

TEST_GROUP(ExceptionTest) {};

TEST(ExceptionTest, testGetMessage) {
    const char* message = "Something went horribly wrong";
    Exception exception = Exception(message);
    CHECK_EQUAL(message, exception.getMessage());
}
