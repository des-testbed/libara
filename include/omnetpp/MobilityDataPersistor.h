/*
 * $FU-Copyright$
 */

#ifndef MOBILITY_DATA_PERSISTOR_H_
#define MOBILITY_DATA_PERSISTOR_H_

#include "OMNeTARAMacros.h"
#include "OMNeTTime.h"

#include <iostream>
#include <fstream>
#include <string>

#include <clistener.h>

#include "Coord.h"
#include "MobilityBase.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * The class MobilityDataPersistor writes the node mobility to a binary
 * file.
 */
class MobilityDataPersistor : public cIListener {
    public:
        MobilityDataPersistor(MobilityBase* mobility, cModule* hostModule);
        ~MobilityDataPersistor();

        void receiveSignal(cComponent *src, simsignal_t id, cObject *obj);
        void receiveSignal(cComponent *src, simsignal_t id, long l);
        void receiveSignal(cComponent *src, simsignal_t id, double d);
        void receiveSignal(cComponent *src, simsignal_t id, simtime_t t);
        void receiveSignal(cComponent *src, simsignal_t id, const char *s);
        void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t);
        void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l);
        
        void unsubscribe(MobilityBase* mobility);

//        void write(Mobility* routingTable);
        void write(Coord position);

    private:
        bool signalMatches(std::string signal);
        std::string getFileName(cModule* hostModule) const;

        std::string signalName;
        std::ofstream file;
};

OMNETARA_NAMESPACE_END

#endif /* MOBILITY_DATA_PERSISTOR_H_ */
