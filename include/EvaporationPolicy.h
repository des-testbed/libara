/******************************************************************************
 Copyright 2012, The DES-SERT Team, Freie Universität Berlin (FUB).
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

#ifndef EVAPORATION_POLICY_H_
#define EVAPORATION_POLICY_H_

#include <stdint.h>
#include <cstring>
#include <sys/time.h>

#include "Time.h"

namespace ARA { 
   /**
    *
    */
    class EvaporationPolicy {
        public:
            EvaporationPolicy();
            virtual ~EvaporationPolicy();

            /// the method checks how much time has passed since the last access to the routing table
            bool checkForEvaporation();
            /// sets the time until the evaporation is triggered
            void setInterval(int interval);
            /// the method returns the different of the last access and the current date
            int getTimeDifference(struct timeval *now);
            /// the method returns how often the evaporation should take place
            int getFactor();

            /// the method reduces the pheromone value of a routing table entry
            virtual float evaporate(float phi) = 0;
            

        protected:
            /// the factor which indicates how often the evaporation should take place 
            uint8_t factor;
            ///
            Time *time;

        private:
            void determineEvaporationFactor(int timeDifference);
            /// the last access time of the routing table
			struct timeval *lastAccessTime;
            /// the interval which denotes how much time has to pass in order to trigger the evaporation
            int interval;
    };
} /* namespace ARA */

#endif 
