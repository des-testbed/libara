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
#include "omnetpp/RoutingTableDataPersistor.h"
#include "omnetpp/MobilityDataPersistor.h"

#include "INotifiable.h"
#include "NotificationBoard.h"
#include "IInterfaceTable.h"
#include "MobilityBase.h"
#include "Energy.h"

OMNETARA_NAMESPACE_BEGIN

class AbstractOMNeTARAClient: public virtual AbstractNetworkClient, public cSimpleModule, public INotifiable {
    public:
        virtual ~AbstractOMNeTARAClient();

        // some signals for statistics recording
        static simsignal_t PACKET_DELIVERED_SIGNAL;
        static simsignal_t PACKET_NOT_DELIVERED_SIGNAL;
        static simsignal_t ROUTE_FAILURE_SIGNAL;
        static simsignal_t DROP_PACKET_BECAUSE_ENERGY_DEPLETED;

        IInterfaceTable* getInterfaceTable();

    protected:
        virtual void initialize(int stage);
        void initializeNetworkInterfacesOf(AbstractARAClient* client, OMNeTConfiguration& configuration);

        /**
         * Is called at the end of each simulation run and records some basic statistics.
         */
        virtual void finish();

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
        virtual void takeAndSend(cMessage* msg, cGate* gate, double sendDelay = 0);

        /**
         * The given packet is sent to the next upper layer.
         * Please note that this method is responsible for deleting the given packet (or delegating
         * this responsibility to another method)
         */
        void deliverToSystem(const Packet* packet);

        /**
         * This method is called if the route discovery is unsuccessful and not route to the packets
         * destination can be established. The task of this method is to notify the upper layers
         * about this event and delete the packet.
         */
        virtual void packetNotDeliverable(const Packet* packet);

        /**
         * Called by the NotificationBoard whenever a change of a category
         * occurs to which this client has subscribed.
         */
        virtual void receiveChangeNotification(int category, const cObject* details);

        /**
         * This handles the broken link in some fashion.
         * It should return true if the link breakage results in a complete route failure.
         * Return false otherwise
         */
        virtual bool handleBrokenOMNeTLink(OMNeTPacket* packet, AddressPtr receiverAddress, NetworkInterface* interface) = 0;

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

        void persistRoutingTableData();

        /**
         * This will record a detected routing loop for statistical purposes.
         */
        void recordDetectedRoutingloop();

        void recordPacketWithZeroTTL();

    private:
        void setPositionFromParameters();
        int getNewNodePosition(const char* positionParameter, int maxPosition, int minPosition);
        void handleBatteryStatusChange(Energy* energyInformation);


        // some statistics collection
        int nrOfDeliverablePackets = 0;
        int nrOfNotDeliverablePackets = 0;
        SimTime nodeEnergyDepletionTimestamp = -1;
        cOutVector energyLevelOutVector;

        MobilityDataPersistor* mobilityDataPersistor = nullptr;
        RoutingTableDataPersistor* routingTablePersistor = nullptr;

    protected:
        NotificationBoard* notificationBoard;
        IInterfaceTable* interfaceTable;
        MobilityBase* mobility;
        ARANetworkConfigurator* networkConfig;

        // node battery
        double maximumBatteryLevel;
        bool hasEnoughBattery = true;
        int currentEnergyLevel;

    friend class OMNeTGate;
};

OMNETARA_NAMESPACE_END

#endif /* ABSTRACT_OMNET_ARA_CLIENT_H_ */
