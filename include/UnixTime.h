/*
 * $FU-Copyright$
 */

#ifndef UNIXTIME_H_
#define UNIXTIME_H_

#include "Time.h"

#include <sys/time.h>

namespace ARA {

    /**
     * This is the unix timestamp implementation of the abstract Time class.
     * It internally uses timeval
     */
    class UnixTime : public Time {

        public:
            UnixTime(long seconds=0, long int microsonds=0);
            UnixTime(struct timeval timestamp);

            bool operator==(const UnixTime& otherTime) const;

            long getSeconds() const;
            long getMilliSeconds() const;

            /**
             * This method provides the subtract operation for two timestamps
             * and stores the result in a new instance of class Time.
             */
            Time* subtract(const Time* right) const;

            /**
             * Returns the timeval timestamp which consists of the seconds
             * and milliseconds since the 1st of January 1970 (unix time).
             */
            timeval getTimestamp() const;

            void setTimestamp(timeval timestamp);

            void setToCurrentTime();

        private:
            timeval getTimeDifference(const UnixTime* right) const;
            /**
             * The current time using a timeval struct which consists of the
             * seconds and milliseconds since the 1st of January 1970 (unix
             * time).
             */
            timeval timestamp;
    };
}

#endif 

