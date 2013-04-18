/*
 * $FU-Copyright$
 */

#ifndef OMNETARACLIENT_H_
#define OMNETARACLIENT_H_

#include "OMNeTARAMacros.h"

#include "AbstractARAClient.h"
#include "AbstractOMNeTARAClient.h"
#include "MessageDispatcher.h"

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
        ARA();
        ~ARA();

        static simsignal_t PACKET_DELIVERED_SIGNAL;
        static simsignal_t PACKET_NOT_DELIVERED_SIGNAL;
        static simsignal_t ARA_LOOP_DETECTION_SIGNAL;

    protected:
        virtual int numInitStages() const;
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);

        /**
         * The packet should be directed to this node and must be delivered to the local system.
         * Please note that this method is responsible for deleting the given packet (or delegating
         * this responsibility to another method)
         */
        virtual void deliverToSystem(const Packet* packet);

        /**
         * This method is called if the route discovery is unsuccessful and not route to the packets
         * destination can be established. The task of this method is to notify the upper layers
         * about this event and delete the packet.
         */
        virtual void packetNotDeliverable(const Packet* packet);

        virtual void handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface);

        virtual void handleBrokenLink(OMNeTPacket* packet, AddressPtr receiverAddress);

        /**
         * This method is called when the route discovery timer expires.
         */
        virtual void timerHasExpired(Timer* responsibleTimer);

    private:
        int nrOfDeliverablePackets = 0;
        int nrOfNotDeliverablePackets = 0;
        int nrOfDetectedLoops = 0;
        MessageDispatcher* messageDispatcher;

    friend class OMNeTGate;
};

OMNETARA_NAMESPACE_END

#endif /* OMNETARACLIENT_H_ */
