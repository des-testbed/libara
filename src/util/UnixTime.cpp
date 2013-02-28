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

Time* UnixTime::subtract(const Time* right) const {
    const UnixTime* unixTime = dynamic_cast<const UnixTime*>(right);
    if(unixTime) {
        timeval result = this->getTimeDifference(unixTime);
        return new UnixTime(result.tv_sec, result.tv_usec);
    }
    else {
        throw Exception("Can only subtract other UnixTime instances from this UnixTime instance");
    }
}

void UnixTime::setToCurrentTime() {
    gettimeofday(&this->timestamp, 0);
}

/**
 * The method determines the time difference between two timestamps. Here,
 * the member variable "timestamp" and a operand "time". The result is stored
 * in a timeval struct which is passed as an argument to the function
 *
 * The code is from the libc manual:
 *   http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 *
 * @param right in The right operand of the computation.
 * @param result out The data structure which will hold the result of the
 *   operation
 */
timeval UnixTime::getTimeDifference(const UnixTime* right) const {
    // copy the content of the right operand
    timeval r;
    r.tv_sec = right->getTimestamp().tv_sec;
    r.tv_usec = right->getTimestamp().tv_usec;

    if (this->timestamp.tv_usec < r.tv_usec) {
        int nanoSeconds = (r.tv_usec - this->timestamp.tv_usec) / 1000000 + 1;
        r.tv_usec -= (1000000 * nanoSeconds);
        r.tv_sec  += nanoSeconds;
    }

    if (this->timestamp.tv_usec - r.tv_usec > 1000000) {
        int nanoSeconds = (this->timestamp.tv_usec - r.tv_usec) / 1000000;
        r.tv_usec += (1000000 * nanoSeconds);
        r.tv_sec -= nanoSeconds;
    }

    timeval result;
    result.tv_sec = this->timestamp.tv_sec - r.tv_sec;
    result.tv_usec = this->timestamp.tv_usec - r.tv_usec;

    return result;
}
