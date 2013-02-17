/******************************************************************************
 Copyright 2012, The DES-ARA-SIM Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große, Michael Frey
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/

#ifndef TIME_H_
#define TIME_H_

#include <cstring>
#include <sys/time.h>

namespace ARA {
    /**
     * The class provides methods for determining the difference between 
     * two timestamps.
     */
    class Time {
        public:
            Time();
            Time(int seconds, long int microsonds);
            Time(struct timeval timestamp);

            /** The copy constructor of class Time */ 
            Time(const Time& other);

            virtual ~Time();
 
            /**
             * The operator provides the subtract operation for two timestamps
             * and stores the result in a new instance of class Time.
             */ 
            virtual Time operator-(const Time& right);

            /**
             * The operator provides the subtract operation for two timestamps
             * and stores the result in the left operand.
             */ 
            virtual Time operator-=(const Time& right);

            /**
             * The method returns the the timestamp which is encapsulated
             * by the class in seconds.
             */
            virtual int toSeconds();

            /**
             * The method returns the the timestamp which is encapsulated
             * by the class in milliseconds.
             */
            virtual long int toMilliseconds();

            /**
             * The method returns the timestamp which is encapsulated
             * by the class.
             */
            struct timeval getTimestamp() const;

            void setTimestamp(struct timeval timestamp);

            /**
             * The method checks if the timestamp is initialized.
             */
            virtual bool isInitialized();


            virtual void update();
            virtual void update(Time timestamp);
 
            /**
             * The method initializes the timestamp.
             */
            virtual void initialize();

        private:
            struct timeval getTimeDifference(const Time& right);

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

