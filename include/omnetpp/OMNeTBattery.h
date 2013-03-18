/*
 * $FU-Copyright$
 */

#ifndef OMNET_BATTERY_H_
#define OMNET_BATTERY_H_

#include <algorithm>
#include <omnetpp.h>

#include "InetSimpleBattery.h"

namespace ARA {
    namespace omnetpp {
        /**
         * The class represents a simple implementation of a battery. It is
         * based on the battery interface as provided in the INETMANET package. 
         */
        class OMNeTBattery : public InetSimpleBattery {
            public:
                OMNeTBattery();
                virtual ~OMNeTBattery();

                virtual int registerDevice(cObject *id,int numAccts);
                virtual void registerWirelessDevice(int id,double mUsageRadioIdle,double mUsageRadioRecv,double mUsageRadioSend,double mUsageRadioSleep);

                virtual void initialize(int stage);
                virtual int numInitStages() const {return 2;}
                virtual void finish();
                virtual void handleMessage(cMessage *msg);

            protected:

        };
    }
}

#endif

