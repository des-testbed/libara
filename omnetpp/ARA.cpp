/*
 * $FU-Copyright$
 */

#include "omnetpp/ARA.h"
#include "omnetpp/OMNeTPacket.h"

OMNETARA_NAMESPACE_BEGIN

// Register the class with the OMNeT++ simulation
Define_Module(ARA);

ARA::ARA() {
    messageDispatcher = new MessageDispatcher(this, this);
}

ARA::~ARA() {
    delete messageDispatcher;
}

int ARA::numInitStages() const {
    return 5;
}

void ARA::initialize(int stage) {
    if(stage == 4) {
        AbstractOMNeTARAClient::initialize();
        OMNeTConfiguration config = OMNeTConfiguration(this);
        setLogger(config.getLogger());

        AbstractARAClient::initialize(config, config.getRoutingTable());
        initializeNetworkInterfacesOf(this, config);
    }
}

void ARA::handleMessage(cMessage* message) {
    messageDispatcher->dispatch(message);
}

void ARA::deliverToSystem(const Packet* packet) {
    sendToUpperLayer(packet);
}

void ARA::packetNotDeliverable(const Packet* packet) {
    nrOfNotDeliverablePackets++;
    //TODO report to upper layer
}

void ARA::finish() {
    recordScalar("nrOfNotDeliverablePackets", nrOfNotDeliverablePackets);
}


void ARA::handleBrokenLink(OMNeTPacket* packet, AddressPtr receiverAddress) {
    // TODO this does only work if we have only one network interface card
    NetworkInterface* interface = getNetworkInterface(0);
    handleRouteFailure(packet, receiverAddress, interface);
}

OMNETARA_NAMESPACE_END
