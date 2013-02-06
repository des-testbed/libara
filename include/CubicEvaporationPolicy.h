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

#ifndef LINEAR_EVAPORATION_POLICY_H_
#define LINEAR_EVAPORATION_POLICY_H_

#include <cmath>
#include <cstdlib>
#include <stdint.h>

#include "EvaporationPolicy.h"

namespace ARA { 
   /**
    * This class provides the linear evaporation function of the ant routing algorithm (ARA).
    */
    class CubicEvaporationPolicy : public EvaporationPolicy {
        public:
			CubicEvaporationPolicy();
            ~CubicEvaporationPolicy();
            ///
//            bool checkForEvaporation();
            /// the method reduces the pheromone value of a routing table entry
            float evaporate(float phi);
            /// sets the time until the evaporation is triggered
 //           void setInterval(int interval);

        private:
 //           void determineEvaporationFactor(int timeDifference);
            /// the last access time of the routing table
//			struct timeval *lastAccessTime;
            /// 
            float plateau;
            ///
            float slow;
            /// 
            float reduction;
            /// the threshold which denotes at what point the pheromone level is set to 0
            float threshold;
    };
} /* namespace ARA */

#endif 
