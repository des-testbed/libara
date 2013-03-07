/*
 * $FU-Copyright$
 */

#include "UnixTime.h"
#include "Exception.h"

using namespace ARA;

UnixTime::UnixTime(long seconds, long int microseconds) {
   timestamp.tv_sec = seconds;
   timestamp.tv_usec = microseconds;
}

UnixTime::UnixTime(struct timeval timestamp) {
    this->timestamp = timestamp;
}

timeval UnixTime::getTimestamp() const {
    return timestamp;
}

bool UnixTime::operator==(const UnixTime& otherTime) const {
    return this->timestamp.tv_sec == otherTime.timestamp.tv_sec
        && this->timestamp.tv_usec == otherTime.timestamp.tv_usec;
}

long UnixTime::getSeconds() const {
    return timestamp.tv_sec;
}

long UnixTime::getMilliSeconds() const {
    return timestamp.tv_usec;
}

long UnixTime::getDifferenceInMilliSeconds(const Time* otherTime) const {
    const UnixTime* otherUnixTime = dynamic_cast<const UnixTime*>(otherTime);
    if(otherUnixTime != 0) {
        long seconds = this->timestamp.tv_sec - otherUnixTime->timestamp.tv_sec;
        long milliSeconds = this->timestamp.tv_usec - otherUnixTime->timestamp.tv_usec;
        if(milliSeconds < 0) {
            seconds--;
            milliSeconds = 1000 + milliSeconds;
        }
        return seconds * 1000 + milliSeconds;
    }
    else {
        throw "UnixTime can only calculate the difference in respect to another UnixTime";
    }
}

void UnixTime::setToCurrentTime() {
    gettimeofday(&this->timestamp, 0);
}
