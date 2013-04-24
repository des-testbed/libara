/*
 * $FU-Copyright$
 */

#include "omnetpp/ARA.h"
#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/PacketFactory.h"
#include "omnetpp/RoutingTableWatcher.h"

OMNETARA_NAMESPACE_BEGIN

// Register the class with the OMNeT++ simulation
Define_Module(ARA);

simsignal_t ARA::PACKET_DELIVERED_SIGNAL = SIMSIGNAL_NULL;
simsignal_t ARA::PACKET_NOT_DELIVERED_SIGNAL = SIMSIGNAL_NULL;
simsignal_t ARA::ARA_LOOP_DETECTION_SIGNAL = SIMSIGNAL_NULL;
simsignal_t ARA::ARA_BROKEN_LINK_SIGNAL = SIMSIGNAL_NULL;

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
        PacketFactory* packetFactory = new PacketFactory(config.getMaxTTL());

        messageDispatcher->setPacketFactory(packetFactory);
        AbstractARAClient::initialize(config, config.getRoutingTable(), packetFactory);
        initializeNetworkInterfacesOf(this, config);

        new RoutingTableWatcher(routingTable);
        WATCH(nrOfDeliverablePackets);
        WATCH(nrOfNotDeliverablePackets);
        WATCH(nrOfDetectedLoops);
        PACKET_DELIVERED_SIGNAL = registerSignal("packetDelivered");
        PACKET_NOT_DELIVERED_SIGNAL = registerSignal("packetUnDeliverable");
        ARA_LOOP_DETECTION_SIGNAL = registerSignal("routingLoopDetected");
        ARA_BROKEN_LINK_SIGNAL = registerSignal("brokenLink");
    }
}

void ARA::handleMessage(cMessage* message) {
    messageDispatcher->dispatch(message);
}

void ARA::deliverToSystem(const Packet* packet) {
    sendToUpperLayer(packet);
    nrOfDeliverablePackets++;
    emit(PACKET_DELIVERED_SIGNAL, 1);
}

void ARA::packetNotDeliverable(const Packet* packet) {
    delete packet;
    nrOfNotDeliverablePackets++;
    emit(PACKET_NOT_DELIVERED_SIGNAL, 1);
}

void ARA::handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface) {
    AbstractARAClient::handleDuplicateErrorPacket(packet, interface);
    nrOfDetectedLoops++;
    emit(ARA_LOOP_DETECTION_SIGNAL, 1);
}

void ARA::handleBrokenLink(OMNeTPacket* packet, AddressPtr receiverAddress) {
    // TODO this does only work if we have only one network interface card
    NetworkInterface* interface = getNetworkInterface(0);
    AbstractARAClient::handleBrokenLink(packet, receiverAddress, interface);
    emit(ARA_BROKEN_LINK_SIGNAL, 1);
}

void ARA::timerHasExpired(Timer* responsibleTimer) {
    bubble("Route Discovery expired");
    AbstractARAClient::timerHasExpired(responsibleTimer);
}

OMNETARA_NAMESPACE_END
