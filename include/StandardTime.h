/*
 * $FU-Copyright$
 */

#ifndef _STANDARD_TIME_H_
#define _STANDARD_TIME_H_

#include "ARAMacros.h"
#include "Time.h"

#include <chrono>

ARA_NAMESPACE_BEGIN

class StandardTime : public Time {
    public:
        StandardTime();
        StandardTime(std::chrono::time_point<std::chrono::system_clock> timestamp);
        virtual ~StandardTime(){};

        bool operator==(const StandardTime& otherTime) const;

        virtual void setToCurrentTime();

        virtual long getDifferenceInMilliSeconds(const Time* otherTime) const;

        std::chrono::time_point<std::chrono::system_clock> getTimestamp() const;

        long getSeconds() const;
        long getMilliSeconds() const;

    private:
        std::chrono::time_point<std::chrono::system_clock> time;
};

ARA_NAMESPACE_END

#endif
