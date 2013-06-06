/*
 * $FU-Copyright$
 */

#ifndef ABSTRACT_OMNET_ARA_CLIENT_H_
#define ABSTRACT_OMNET_ARA_CLIENT_H_

#include "AbstractNetworkClient.h"

#include "omnetpp/OMNeTARAMacros.h"
#include "omnetpp/OMNeTConfiguration.h"
#include "omnetpp/ARANetworkConfigurator.h"
#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/MobilityDataPersistor.h"

#include "INotifiable.h"
#include "NotificationBoard.h"
#include "IInterfaceTable.h"
#include "MobilityBase.h"

OMNETARA_NAMESPACE_BEGIN

class AbstractOMNeTARAClient: public virtual AbstractNetworkClient, public cSimpleModule, public INotifiable {
    public:
        virtual ~AbstractOMNeTARAClient();
        IInterfaceTable* getInterfaceTable();

    protected:
        virtual void initialize();
        void initializeNetworkInterfacesOf(AbstractARAClient* client, OMNeTConfiguration& configuration);

        /**
         * Handles a message from the OMNeT++ simulation.
         */
        virtual void handleMessage(cMessage *msg);

        /**
         * Checks whether the given cMessage cam from the upper layer.
         */
        bool isFromUpperLayer(cMessage* message);

        /**
         * Extracts the IPv4ControlInfo object from the given message
         * and creates a new data packet from the extracted destination and source.
         * The Packet is then send via AbstractARAClient::sendPacket(..)
         */
        void handleUpperLayerMessage(cMessage* message);

        /**
         * This casts the given message to OMNeTPacket and dispatches it to the correct arrival gate
         */
        void handleARAMessage(cMessage* message);

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

        virtual void handleBrokenOMNeTLink(OMNeTPacket* packet, AddressPtr receiverAddress) = 0;

        /**
         * Determines the host module which acts as a container to this simple module.
         * A host module is defined as the next compound module up the model hierarchy
         * that has a @node property set
         */
        cModule* findHost() const;

        /**
         * Returns the address of the clients interface.
         * Note that this does *not* work with multiple interfaces and will
         * throw an exception if more than one (non loop-back) interface is found
         * to warn the user.
         */
        AddressPtr getLocalAddress();

    private:
        void setPositionFromParameters();
        int getNewNodePosition(const char* positionParameter, int maxPosition, int minPosition);
        bool mobilityTraceEnabled;
        MobilityDataPersistor* mobilityDataPersistor;

    protected:
        NotificationBoard* notificationBoard;
        IInterfaceTable* interfaceTable;
        MobilityBase* mobility;
        ARANetworkConfigurator* networkConfig;

    friend class OMNeTGate;
};

OMNETARA_NAMESPACE_END

#endif /* ABSTRACT_OMNET_ARA_CLIENT_H_ */
