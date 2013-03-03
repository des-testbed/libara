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

            virtual void setToCurrentTime();
            virtual long getDifferenceInMilliSeconds(const Time* right) const;

            bool operator==(const UnixTime& otherTime) const;

            long getSeconds() const;
            long getMilliSeconds() const;

            /**
             * Returns the timeval timestamp which consists of the seconds
             * and milliseconds since the 1st of January 1970 (unix time).
             */
            timeval getTimestamp() const;

            void setTimestamp(timeval timestamp);


        private:
            /**
             * The current time using a timeval struct which consists of the
             * seconds and milliseconds since the 1st of January 1970 (unix
             * time).
             */
            timeval timestamp;
    };
}

#endif 

