/*
 * $FU-Copyright$
 */

#ifndef TIME_H_
#define TIME_H_

#include <sys/time.h>

namespace ARA {
    /**
     * The class provides methods for determining the difference between 
     * two timestamps.
     */
    class Time {
        public:
            Time(); // standard constructor will be removed shortly
            Time(int seconds, long int microsonds);
            Time(struct timeval timestamp);

            /** The copy constructor of class Time */ 
            Time(const Time& other);

            virtual ~Time();
 
            /**
             * Subtracts another time from this time and returns the result
             * in a new Time instance.
             */
            virtual Time subtract(const Time& right) const;

            /**
             * The method returns the the timestamp which is encapsulated
             * by the class in seconds.
             */
            virtual int getSeconds() const;

            /**
             * Returns the milliseconds part of this time as an integer value
             */
            virtual long getMilliSeconds() const;

            /**
             * The method returns the timestamp which is encapsulated
             * by the class.
             */
            struct timeval getTimestamp() const;

            void setTimestamp(struct timeval timestamp);

            virtual void setToCurrentTime();
            virtual void update(Time timestamp);

        private:
            struct timeval getTimeDifference(const Time& right) const;

            /**
             * The member variable represents the current time using
             * a timeval struct (which consists of the seconds and
             * milliseconds since the 1st of January 1970 (unix 
             * time)).
             */ 
            struct timeval timestamp;
    };
}

#endif 

