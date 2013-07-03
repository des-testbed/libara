/*
 * $FU-Copyright$
 */

#include "omnetpp/ARA.h"
#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/PacketFactory.h"
#include "omnetpp/OMNeTGate.h"

OMNETARA_NAMESPACE_BEGIN

// Register the class with the OMNeT++ simulation
Define_Module(ARA);

simsignal_t ARA::LOOP_DETECTION_SIGNAL = SIMSIGNAL_NULL;
simsignal_t ARA::DROP_PACKET_WITH_ZERO_TTL = SIMSIGNAL_NULL;
simsignal_t ARA::ROUTE_FAILURE_NO_HOP = SIMSIGNAL_NULL;
simsignal_t ARA::NEW_ROUTE_DISCOVERY = SIMSIGNAL_NULL;
simsignal_t ARA::ROUTE_FAILURE_NEXT_HOP_IS_SENDER = SIMSIGNAL_NULL;

ARA::~ARA() {
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
    AbstractOMNeTARAClient::initialize(stage);
    if(stage == 4) {
        OMNeTConfiguration config = OMNeTConfiguration(this);
        setLogger(config.getLogger());
        PacketFactory* packetFactory = new PacketFactory(config.getMaxTTL());
        AbstractARAClient::initialize(config, packetFactory);
        initializeNetworkInterfacesOf(this, config);
        maximumBatteryLevel = config.getMaximumBatteryLevel();

        WATCH(nrOfDetectedLoops);
        LOOP_DETECTION_SIGNAL = registerSignal("routingLoopDetected");
        DROP_PACKET_WITH_ZERO_TTL = registerSignal("dropZeroTTLPacket");
        ROUTE_FAILURE_NO_HOP = registerSignal("routeFailureNoHopAvailable");
        NEW_ROUTE_DISCOVERY = registerSignal("newRouteDiscovery");
        ROUTE_FAILURE_NEXT_HOP_IS_SENDER =  registerSignal("routeFailureNextHopIsSender");
    }
}

void ARA::receivePacket(Packet* packet, NetworkInterface* interface) {
    AbstractARAClient::receivePacket(packet, interface);
    persistRoutingTableData();
}

void ARA::handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface) {
    AbstractARAClient::handleDuplicateErrorPacket(packet, interface);
    nrOfDetectedLoops++;
    emit(LOOP_DETECTION_SIGNAL, 1);
}

bool ARA::handleBrokenOMNeTLink(OMNeTPacket* packet, AddressPtr receiverAddress, NetworkInterface* interface) {
    return AbstractARAClient::handleBrokenLink(packet, receiverAddress, interface);
}

void ARA::handlePacketWithZeroTTL(Packet* packet) {
    AbstractARAClient::handlePacketWithZeroTTL(packet);

    if(packet->isDataPacket()) {
        emit(DROP_PACKET_WITH_ZERO_TTL, 1);
    }
}

void ARA::handleNonSourceRouteDiscovery(Packet* packet) {
    if(routingTable->isDeliverable(packet->getDestination())) {
        // can not be sent because the only known next hop is the sender of this packet
        emit(ROUTE_FAILURE_NEXT_HOP_IS_SENDER, 1);
    }
    else {
        // can not be sent because there really is no known next hop
        emit(ROUTE_FAILURE_NO_HOP, 1);
    }
    AbstractARAClient::handleNonSourceRouteDiscovery(packet);
}

void ARA::startNewRouteDiscovery(Packet* packet) {
    emit(NEW_ROUTE_DISCOVERY, 1);
    AbstractARAClient::startNewRouteDiscovery(packet);
}

OMNETARA_NAMESPACE_END
