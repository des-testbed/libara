/*
 * $FU-Copyright$
 */

#ifndef OMNETGATE_H_
#define OMNETGATE_H_

#include "omnetpp/OMNeTARAMacros.h"
#include "omnetpp/ARANetworkConfigurator.h"

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
        OMNeTGate(AbstractOMNeTARAClient* module, AbstractARAClient* araClient, cGate* gate, InterfaceEntry* interfaceEntry);

        void send(const Packet* packet, std::shared_ptr<Address> recipient);
        void send(const Packet* packet, std::shared_ptr<Address> recipient, double sendDelay);

        void broadcast(const Packet* packet);
        bool equals(NetworkInterface* interface);

        int64 getNrOfSentDataBits();
        int64 getNrOfSentControlBits();
        unsigned int getNrOfControlPackets();
        unsigned int getNrOfDataPackets();

    private:
        std::shared_ptr<OMNeTAddress> getNextHopAddress(std::shared_ptr<Address> recipient);

    private:
        AbstractOMNeTARAClient* omnetARAModule;
        ::ARA::PacketFactory* packetFactory;
        ARANetworkConfigurator* networkConfig;
        cGate* outGate;
        int interfaceID;

        // this is only for statistics
        int64 nrOfSentControlBits;
        int64 nrOfSentDataBits;
        unsigned int nrOfSentControlPackets;
        unsigned int nrOfSentDataPackets;
};

OMNETARA_NAMESPACE_END

#endif /* OMNETGATE_H_ */
