/*
 * $FU-Copyright$
 */

#include "TimeMock.h"

#include <iostream>
#include <typeinfo>

using namespace ARA;

TimeMock TimeMock::currentTime = TimeMock();

TimeMock::TimeMock(long seconds, long int milliseconds) {
    this->seconds = seconds;
    this->milliseconds = milliseconds;
}

void TimeMock::setToCurrentTime() {
    this->seconds = currentTime.seconds;
    this->milliseconds = currentTime.milliseconds;
}

long TimeMock::getDifferenceInMilliSeconds(const Time* otherTime) const {
    const TimeMock* otherTimeMock = dynamic_cast<const TimeMock*>(otherTime);
    if(otherTimeMock != NULL) {
        long seconds = this->seconds - otherTimeMock->seconds;
        long milliSeconds = this->milliseconds - otherTimeMock->milliseconds;
        if(milliSeconds < 0) {
            seconds--;
            milliSeconds = 1000 + milliSeconds;
        }
        return seconds * 1000 + milliSeconds;
    }
    else {
        throw "TimeMock can only calculate the difference in respect to another TimeMock";
    }
}

long TimeMock::getSeconds() const {
    return seconds;
}

long TimeMock::getMilliSeconds() const {
    return milliseconds;
}
