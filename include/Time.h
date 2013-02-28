/*
 * $FU-Copyright$
 */

#ifndef TIME_H_
#define TIME_H_

namespace ARA {

    /**
     * This class is an abstraction of a general concept of time.
     * It is used to perform basic time calculation like time
     * differences (how much time has actually passed) or conversion to
     * standard time units (like seconds or milliseconds).
     *
     * We need this additional layer of abstraction because libARA is supposed
     * to run in very different environments. First of all a unix routing daemon
     * will most likely be based on unix timestamps while a discrete event simulator
     * like OMNeT++ or NS-2 will implement a completely different concept of time.
     */
    class Time {
        public:
            virtual ~Time() {};
 
            /**
             * Set the internally used time value to the current system time.
             */
            virtual void setToCurrentTime() = 0;

            /**
             * Subtracts another time from this time and returns the result
             * in a new Time instance which will have to be deleted by the
             * invoking object.
             */
            virtual Time* subtract(const Time* right) const = 0;

            /**
             * The method returns the the timestamp which is encapsulated
             * by the class in seconds.
             */
            virtual long getSeconds() const = 0;

            /**
             * Returns the milliseconds part of this time as an integer value
             */
            virtual long getMilliSeconds() const = 0;
    };
}

#endif 

