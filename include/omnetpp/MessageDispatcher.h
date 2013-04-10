/*
 * $FU-Copyright$
 */

#ifndef OMNET_MESSAGE_DISPATCHER_H_
#define OMNET_MESSAGE_DISPATCHER_H_

#include "omnetpp/OMNeTARAMacros.h"
#include "omnetpp/AbstractOMNeTARAClient.h"
#include "AbstractARAClient.h"
#include "InterfaceEntry.h"

OMNETARA_NAMESPACE_BEGIN

class ARA;

class MessageDispatcher {
    public:
        MessageDispatcher(AbstractOMNeTARAClient* module, AbstractARAClient* araClient);
        void dispatch(cMessage* message);

        bool isFromUpperLayer(cMessage* message);
        bool isARPMessage(cMessage* message);
        bool isARAMessage(cMessage* message);

        void setMaxTTL(int n);

    private:
        void handleUpperLayerMessage(cMessage* message);
        void handleARP(cMessage* message);
        void handleARA(cMessage* message);

        InterfaceEntry* getSourceInterfaceFrom(cMessage* message);

        AbstractOMNeTARAClient* module;
        AbstractARAClient* araClient;
        int maxTTL= 20;
};

OMNETARA_NAMESPACE_END
#endif
