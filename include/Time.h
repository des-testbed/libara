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
             * as numerical value in milliseconds.
             */
            virtual long getDifferenceInMilliSeconds(const Time* right) const = 0;

    };
}

#endif 

