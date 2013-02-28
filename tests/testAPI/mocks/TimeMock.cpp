#include "TimeMock.h"

#include <iostream>
#include <typeinfo>

using namespace ARA;

TimeMock::TimeMock() {
    this->seconds = 0;
    this->milliseconds = 0;
}

TimeMock::TimeMock(long seconds, long int milliseconds) {
    this->seconds = seconds;
    this->milliseconds = milliseconds;
}

void TimeMock::setToCurrentTime() {

}

Time* TimeMock::subtract(const Time* right) const {
    return new TimeMock();
}

long TimeMock::getSeconds() const {
    return seconds;
}

long TimeMock::getMilliSeconds() const {
    return milliseconds;
}

void TimeMock::letTimePass(long milliSeconds) {
    long passedSeconds = (this->milliseconds + milliSeconds)/1000;
    long passedMilliseconds = milliSeconds-passedSeconds*1000;

    this->seconds += passedSeconds;
    this->milliseconds += passedMilliseconds;
}
