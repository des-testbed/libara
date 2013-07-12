/*
 * $FU-Copyright$
 */

#ifndef TRAFFIC_GENERATOR_H_
#define TRAFFIC_GENERATOR_H_

#include "OMNeTARAMacros.h"
#include "TrafGen.h"
#include "INotifiable.h"
#include "IPv4Address.h"
#include "IPv4Datagram.h"
#include "IPv4ControlInfo.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * The class provides a traffic generator for the ARA/EARA routing
 * protocols. It is based on the INETMANET class TrafGen.
 */
class TrafficGenerator : public TrafGen, public INotifiable {
    public:
        virtual void initialize(int level);
        virtual void receiveChangeNotification(int category, const cObject* details);

    protected:
        virtual void handleSelfMsg(cMessage* message);
        virtual void SendTraf(cPacket* msg, const char* destination);
        void sendTraffic(cPacket* message, const char* destination);

    private:
        bool isEnergyDepleted;
        int nrOfPacketsToSend;
        int nrOfSentMessages = 0;
};

OMNETARA_NAMESPACE_END
#endif
