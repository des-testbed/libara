/*
 * $FU-Copyright$
 */

#include "StandardTime.h"

ARA_NAMESPACE_BEGIN

StandardTime::StandardTime() {
    //FIXME do we need this? should there be initialization?
}

StandardTime::StandardTime(std::chrono::time_point<std::chrono::system_clock> timestamp):time(timestamp){

}

long StandardTime::getSeconds() const {
    return std::chrono::duration_cast<std::chrono::seconds>(this->time.time_since_epoch()).count();
}

long StandardTime::getMilliSeconds() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(this->time.time_since_epoch()).count();
}

bool StandardTime::operator==(const StandardTime& otherTime) const {
    return this->time == otherTime.getTimestamp();
}

long StandardTime::getDifferenceInMilliSeconds(const Time* otherTime) const {
     const StandardTime* otherStandardTime = dynamic_cast<const StandardTime*>(otherTime);
     if (otherStandardTime != 0) {
         long elapsedMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(otherStandardTime->getTimestamp() - this->time).count();
         return elapsedMilliSeconds;
     } else {
         throw "StandardTime can only calculate the difference in respect to another StandardTime";
     }
}

std::chrono::time_point<std::chrono::system_clock> StandardTime::getTimestamp() const {
    return this->time;
}

void StandardTime::setToCurrentTime() {
    this->time = std::chrono::system_clock::now();
}


ARA_NAMESPACE_END
