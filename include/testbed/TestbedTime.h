/*
 * $FU-Copyright$
 */

#ifndef _TESTBED_TIME_H_
#define _TESTBED_TIME_H_

#include <ctime>
#include <chrono>

#include "Time.h"
#include "Testbed.h"

TESTBED_NAMESPACE_BEGIN

class TestbedTime : public Time {
    public:
        TestbedTime();
        TestbedTime(std::chrono::time_point<std::chrono::system_clock> timestamp);
        virtual ~TestbedTime(){};

        bool operator==(const TestbedTime& otherTime) const;

        virtual void setToCurrentTime();

        virtual long getDifferenceInMilliSeconds(const Time* otherTime) const;

        std::chrono::time_point<std::chrono::system_clock> getTimestamp() const;

        long getSeconds() const;
        long getMilliSeconds() const;

    private:
        std::chrono::time_point<std::chrono::system_clock> time;
};

TESTBED_NAMESPACE_END

#endif
