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

#ifndef OMNET_TIME_H_
#define OMNET_TIME_H_

#include <cmath>
#include <omnetpp.h>
#include <simtime.h>

#include "Time.h"

namespace ARA {
    /**
     * The class provides methods for determining the time difference between 
     * two timestamps in OMNeT++.
     */
    class OMNeTTime : public Time {
        public:
          OMNeTTime();
          OMNeTTime(SimTime timestamp);
          ~OMNeTTime();

          OMNeTTime operator-(const OMNeTTime& right);
          OMNeTTime operator-=(const OMNeTTime& right);

          ///
          virtual int toSeconds();
          ///
          virtual long int toMilliseconds();
          ///
          SimTime getTimestamp() const;

          /**
           * The method checks if the timestamp is initialized.
           */
          virtual bool isInitialized();

          virtual void update();

          /**
           * The method initializes the timestamp.
           */
          virtual void initialize();

        private:
          /// 
          int convertSimulationTime(int scaleExponent);

          SimTime timestamp;
    };
}

#endif 

