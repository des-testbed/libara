/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_OMNET_ARA_CLIENT_H_
#define ABSTRACT_OMNET_ARA_CLIENT_H_

#include "omnetpp/OMNeTARAMacros.h"
#include "omnetpp/OMNeTConfiguration.h"
#include "omnetpp/ARANetworkConfigurator.h"
#include "omnetpp/OMNeTPacket.h"

#include "Packet.h"
#include "INotifiable.h"
#include "NotificationBoard.h"
#include "IInterfaceTable.h"

OMNETARA_NAMESPACE_BEGIN

class AbstractOMNeTARAClient: public cSimpleModule, public INotifiable {
    public:
        virtual ~AbstractOMNeTARAClient() {};
        IInterfaceTable* getInterfaceTable();

    protected:
        virtual void initialize();
        void initializeNetworkInterfacesOf(AbstractARAClient* client, OMNeTConfiguration& configuration);

        /**
         * Method for friend class OMNeTGate.
         * It switches the context to the ARAClient,
         * takes ownership of the packet and sends it to the specified gate
         * with the given delay.
         */
        //TODO rename this to sendToNetwork() and remove the gate parameter
        void takeAndSend(cMessage* msg, cGate* gate, double sendDelay = 0);

        /**
         * The given packet should be to the next upper layer.
         * Please note that this method is responsible for deleting the given packet (or delegating
         * this responsibility to another method)
         */
        void sendToUpperLayer(const Packet* packet);

        /**
         * Called by the NotificationBoard whenever a change of a category
         * occurs to which this client has subscribed.
         */
        virtual void receiveChangeNotification(int category, const cObject* details);

        virtual void handleBrokenLink(OMNeTPacket* packet, AddressPtr receiverAddress) = 0;

    protected:
        NotificationBoard* notificationBoard;
        IInterfaceTable* interfaceTable;
        ARANetworkConfigurator* networkConfig;

    friend class OMNeTGate;
};

OMNETARA_NAMESPACE_END

#endif /* ABSTRACT_OMNET_ARA_CLIENT_H_ */
