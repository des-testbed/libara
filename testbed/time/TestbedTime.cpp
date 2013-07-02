#include "TestbedTime.h"

TESTBED_NAMESPACE_BEGIN

TestbedTime::TestbedTime(){}

TestbedTime::TestbedTime(std::chrono::time_point<std::chrono::system_clock> timestamp):time(timestamp){ }

long TestbedTime::getSeconds() const {
    return std::chrono::duration_cast<std::chrono::seconds>(this->time.time_since_epoch()).count();
}

long TestbedTime::getMilliSeconds() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(this->time.time_since_epoch()).count();
}

bool TestbedTime::operator==(const TestbedTime& otherTime) const {
    return this->time == otherTime.getTimestamp();
}

long TestbedTime::getDifferenceInMilliSeconds(const Time* otherTime) const {
     const TestbedTime* otherTestbedTime = dynamic_cast<const TestbedTime*>(otherTime);
     if (otherTestbedTime != 0) {
         long elapsedMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(otherTestbedTime->getTimestamp() - this->time).count();
         return elapsedMilliSeconds;
     } else {
         throw "TestbedTime can only calculate the difference in respect to another TestbedTime";
     }
}

std::chrono::time_point<std::chrono::system_clock> TestbedTime::getTimestamp() const {
    return this->time;
}

void TestbedTime::setToCurrentTime() {
    this->time = std::chrono::system_clock::now();
}


TESTBED_NAMESPACE_END
