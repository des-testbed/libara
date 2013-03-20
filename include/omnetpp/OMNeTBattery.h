/*
 * $FU-Copyright$
 */

#ifndef OMNET_BATTERY_H_
#define OMNET_BATTERY_H_

#include <algorithm>
#include <omnetpp.h>

#include "Energy.h"
#include "InetSimpleBattery.h"

namespace ARA {
    namespace omnetpp {
        /**
         * The class represents a simple implementation of a battery. It is
         * based on the battery interface as provided in the INETMANET package. 
         */
        class OMNeTBattery : public InetSimpleBattery {
            public:
                OMNeTBattery(){};
                virtual ~OMNeTBattery(){};

            protected:
                void deductAndCheck();
        };
    }
}

#endif
