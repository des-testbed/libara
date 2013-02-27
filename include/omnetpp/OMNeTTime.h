/*
 * $FU-Copyright$
 */

#ifndef OMNET_TIME_H_
#define OMNET_TIME_H_

#include <omnetpp.h>
#include <cmath>

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

          int getSeconds() const;
          long getMilliseconds() const;

          SimTime getTimestamp() const;

          void setToCurrentTime();

        private:
          int convertSimulationTime(int scaleExponent) const;

          SimTime timestamp;
    };
}

#endif 

