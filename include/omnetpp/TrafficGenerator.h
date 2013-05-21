/*
 * $FU-Copyright$
 */

#ifndef TRAFFIC_GENERATOR_H_
#define TRAFFIC_GENERATOR_H_

#include "OMNeTARAMacros.h"
#include "TrafGen.h"
#include "IPv4Address.h"
#include "IPv4Datagram.h"
#include "IPv4ControlInfo.h"

OMNETARA_NAMESPACE_BEGIN

/**
 * The class provides a traffic generator for the ARA/EARA routing
 * protocols. It is based on the INETMANET class TrafGen.
 */
class TrafficGenerator : public TrafGen {
    public:
        virtual void initialize(int level);
        virtual void finish();

    protected:
        virtual void handleLowerMsg(cPacket* packet);
        virtual void SendTraf(cPacket* msg, const char* destination);
        void sendTraffic(cPacket* message, const char* destination);

    private:
        IPv4Address getLocalAddress();

    private:
        int nrOfPacketsToSend;
        int nrOfSentMessages = 0;
        int nrOfReceivedMessages = 0;
};

OMNETARA_NAMESPACE_END
#endif
