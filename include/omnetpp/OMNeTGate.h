/*
 * $FU-Copyright$
 */

#ifndef OMNETGATE_H_
#define OMNETGATE_H_

#include "omnetpp/OMNeTARAMacros.h"

#include "AbstractNetworkInterface.h"
#include "omnetpp/AbstractOMNeTARAClient.h"
#include "omnetpp/OMNeTAddress.h"
#include "AbstractARAClient.h"
#include "InterfaceEntry.h"
#include "PacketFactory.h"

OMNETARA_NAMESPACE_BEGIN

class ARA;

//TODO write some more documentation for this class
class OMNeTGate: public AbstractNetworkInterface {
    public:
        OMNeTGate(AbstractOMNeTARAClient* module, AbstractARAClient* araClient, cGate* gate, InterfaceEntry* interfaceEntry, double broadCastDelay, double uniCastDelay);

        void send(const Packet* packet, std::shared_ptr<Address> recipient);
        void send(const Packet* packet, std::shared_ptr<Address> recipient, double sendDelay);

        void broadcast(const Packet* packet);
        bool equals(NetworkInterface* interface);

    private:
        std::shared_ptr<OMNeTAddress> getNextHopAddress(std::shared_ptr<Address> recipient);

    private:
        AbstractOMNeTARAClient* omnetARAModule;
        ::ARA::PacketFactory* packetFactory;
        cGate* gateToARP;
        int interfaceID;

        /**
         * The delay in seconds that is added to broadcast operations to
         * prevent packet collision by perfect synchronization in the simulation
         */
        double broadCastDelay;

        /**
         * The delay in seconds that is added to unicast messaged.
         * It is used to model processing time and prevents perfect
         * event synchronization which would lead to packet collisions
         */
        double uniCastDelay;
};

OMNETARA_NAMESPACE_END

#endif /* OMNETGATE_H_ */
