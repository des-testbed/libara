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
simsignal_t ARA::LOOP_DETECTION_SIGNAL = SIMSIGNAL_NULL;
simsignal_t ARA::ROUTE_FAILURE_SIGNAL = SIMSIGNAL_NULL;
simsignal_t ARA::DROP_PACKET_WITH_ZERO_TTL = SIMSIGNAL_NULL;
simsignal_t ARA::NON_SOURCE_ROUTE_DISCOVERY = SIMSIGNAL_NULL;
simsignal_t ARA::NEW_ROUTE_DISCOVERY = SIMSIGNAL_NULL;

ARA::~ARA() {
    delete routingTablePersistor;

    /* We set the policies to nullptr in order to prevent the AbstractARAClient from deleting those.
     * This is necessary because the surround omnetpp simulation will attempt to delete those modules
     * because they are SimpleModules which are owned by other compound modules*/

    forwardingPolicy = nullptr;
    evaporationPolicy = nullptr;
    pathReinforcementPolicy = nullptr;
}

int ARA::numInitStages() const {
    return 5;
}

void ARA::initialize(int stage) {
    if(stage == 0) {
        AbstractOMNeTARAClient::initialize();
    }
    else if(stage == 4) {
        OMNeTConfiguration config = OMNeTConfiguration(this);
        setLogger(config.getLogger());
        PacketFactory* packetFactory = new PacketFactory(config.getMaxTTL());
        AbstractARAClient::initialize(config, config.getRoutingTable(), packetFactory);
        initializeNetworkInterfacesOf(this, config);

        routingTablePersistor = new RoutingTableDataPersistor(findHost(), par("routingTableStatisticsUpdate").longValue());
        new RoutingTableWatcher(routingTable);
        WATCH(nrOfDeliverablePackets);
        WATCH(nrOfNotDeliverablePackets);
        WATCH(nrOfDetectedLoops);
        PACKET_DELIVERED_SIGNAL = registerSignal("packetDelivered");
        PACKET_NOT_DELIVERED_SIGNAL = registerSignal("packetUnDeliverable");
        LOOP_DETECTION_SIGNAL = registerSignal("routingLoopDetected");
        ROUTE_FAILURE_SIGNAL = registerSignal("routeFailure");
        DROP_PACKET_WITH_ZERO_TTL = registerSignal("dropZeroTTLPacket");
        NON_SOURCE_ROUTE_DISCOVERY = registerSignal("nonSourceRouteDiscovery");
        NEW_ROUTE_DISCOVERY = registerSignal("newRouteDiscovery");
    }
}

void ARA::receivePacket(Packet* packet, NetworkInterface* interface) {
    AbstractARAClient::receivePacket(packet, interface);
    routingTablePersistor->write(routingTable);
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
    emit(LOOP_DETECTION_SIGNAL, 1);
}

void ARA::handleBrokenOMNeTLink(OMNeTPacket* packet, AddressPtr receiverAddress) {
    // TODO this does only work if we have only one network interface card
    NetworkInterface* interface = getNetworkInterface(0);
    AddressPtr packetSource = packet->getSource();
    AbstractARAClient::handleBrokenLink(packet, receiverAddress, interface);

    if(routingTable->isDeliverable(packet) == false) {
        if(isLocalAddress(packetSource) == false) {
            emit(ROUTE_FAILURE_SIGNAL, 1);
        }
    }
}

void ARA::timerHasExpired(Timer* responsibleTimer) {
    AbstractARAClient::timerHasExpired(responsibleTimer);
}

void ARA::handlePacketWithZeroTTL(Packet* packet) {
    AbstractARAClient::handlePacketWithZeroTTL(packet);

    if(packet->isDataPacket()) {
        emit(DROP_PACKET_WITH_ZERO_TTL, 1);
    }
}

void ARA::handleNonSourceRouteDiscovery(Packet* packet) {
    emit(NON_SOURCE_ROUTE_DISCOVERY, 1);
    AbstractARAClient::handleNonSourceRouteDiscovery(packet);
}

void ARA::startNewRouteDiscovery(Packet* packet) {
    emit(NEW_ROUTE_DISCOVERY, 1);
    AbstractARAClient::startNewRouteDiscovery(packet);
}

void ARA::finish() {
    recordScalar("nrOfTrappedPacketsAfterFinish", packetTrap->getNumberOfTrappedPackets());
}

OMNETARA_NAMESPACE_END
