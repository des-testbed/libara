/*
 * $FU-Copyright$
 */

#ifndef OMNET_TIME_H_
#define OMNET_TIME_H_

#include <omnetpp.h>

#include "Time.h"
#include "platdep/intxtypes.h"

namespace ARA {
namespace omnetpp {

    /**
     * The class provides methods for determining the time difference between 
     * two time stamps in OMNeT++.
     */
    class OMNeTTime : public Time {
        public:
            virtual void setToCurrentTime();
            virtual long getDifferenceInMilliSeconds(const Time* otherTime) const;

            uint64 getRawTime() const;

        private:
            long convertSimulationTime(SimTime time, int wantedScaleExponent) const;

            SimTime timestamp;
    };
}
}

#endif 

