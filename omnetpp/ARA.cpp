/*
 * $FU-Copyright$
 */

#include "omnetpp/ARA.h"
#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/PacketFactory.h"

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
        messageDispatcher->setMaxTTL(config.getMaxTTL());

        AbstractARAClient::initialize(config, config.getRoutingTable(), new PacketFactory());
        initializeNetworkInterfacesOf(this, config);
        WATCH(nrOfNotDeliverablePackets);
        WATCH(nrOfDetectedLoops);
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

void ARA::handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface) {
    AbstractARAClient::handleDuplicateErrorPacket(packet, interface);
    nrOfDetectedLoops++;
}

void ARA::finish() {
    recordScalar("nrOfNotDeliverablePackets", nrOfNotDeliverablePackets);
    recordScalar("nrOfDetectedLoops", nrOfDetectedLoops);
}


void ARA::handleBrokenLink(OMNeTPacket* packet, AddressPtr receiverAddress) {
    // TODO this does only work if we have only one network interface card
    NetworkInterface* interface = getNetworkInterface(0);
    handleRouteFailure(packet, receiverAddress, interface);
}

OMNETARA_NAMESPACE_END
