/*
 * $FU-Copyright$
 */

#ifndef OMNETARACLIENT_H_
#define OMNETARACLIENT_H_

#include "OMNeTARAMacros.h"

#include "AbstractARAClient.h"
#include "AbstractOMNeTARAClient.h"

OMNETARA_NAMESPACE_BEGIN

class MessageDispatcher;

/**
 * The class represents the implementation of the ant routing algorithm (ARA)
 * for the OMNeT++ simulation framework.
 *
 * The algorithm was first published in:
 *
 *  Mesut Guenes, Udo Sorges, and Imed Bouazizi. "ARA-the ant-colony based routing algorithm for MANETs."
 *  Parallel Processing Workshops, 2002. Proceedings. International Conference on. IEEE, 2002.
 */
class ARA: public AbstractARAClient, public AbstractOMNeTARAClient {
    public:
        ~ARA();

        // some signals for statistics recording
        static simsignal_t LOOP_DETECTION_SIGNAL;
        static simsignal_t DROP_PACKET_WITH_ZERO_TTL;
        static simsignal_t ROUTE_FAILURE_NO_HOP;
        static simsignal_t NEW_ROUTE_DISCOVERY;
        static simsignal_t ROUTE_FAILURE_NEXT_HOP_IS_SENDER;

    protected:
        virtual int numInitStages() const;
        virtual void initialize(int stage);

        virtual void handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface);

        virtual bool handleBrokenOMNeTLink(OMNeTPacket* packet, AddressPtr receiverAddress, NetworkInterface* interface);

        virtual void handlePacketWithZeroTTL(Packet* packet);

        virtual void handleNonSourceRouteDiscovery(Packet* packet);

        virtual void startNewRouteDiscovery(Packet* packet);

    private:
        int nrOfDetectedLoops = 0;

    friend class OMNeTGate;
};

OMNETARA_NAMESPACE_END

#endif /* OMNETARACLIENT_H_ */
