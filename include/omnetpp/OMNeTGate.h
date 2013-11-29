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

    private:
        std::shared_ptr<OMNeTAddress> getNextHopAddress(std::shared_ptr<Address> recipient);
        void collectStatistics(const Packet* packet, cPacket* simPacket);

    private:
        AbstractOMNeTARAClient* omnetARAModule;
        ::ARA::PacketFactory* packetFactory;
        ARANetworkConfigurator* networkConfig;
        cGate* outGate;
        int interfaceID;

    public:
        // this is only for statistics
        unsigned int nrOfSentControlPackets;
        unsigned int nrOfSentDataPackets;
        unsigned int nrOfSentFANTs;
        unsigned int nrOfSentBANTs;
        unsigned int nrOfSentPEANTs;
        unsigned int nrOfSentDuplicateErrors;
        unsigned int nrOfSentRouteErrors;
        unsigned int nrOfSentOtherPackets;

        int64 nrOfSentControlBits;
        int64 nrOfSentDataBits;
        int64 nrOfSentFANTBits;
        int64 nrOfSentBANTBits;
        int64 nrOfSentPEANTBits;
        int64 nrOfSentDuplicateErrorBits;
        int64 nrOfSentRouteErrorBits;
        int64 nrOfSentOtherBits;
};

OMNETARA_NAMESPACE_END

#endif /* OMNETGATE_H_ */
