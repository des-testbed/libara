/*
 * $FU-Copyright$
 */

#include "omnetpp/EARA.h"
#include "omnetpp/OMNeTEARAConfiguration.h"
#include "omnetpp/EARAPacketFactory.h"
#include "omnetpp/OMNeTEARAPacket.h"
#include "omnetpp/traffic/TrafficPacket_m.h"
#include "omnetpp/EARARoutingTableWatcher.h"

OMNETARA_NAMESPACE_BEGIN

// Register the class with the OMNeT++ simulation
Define_Module(EARA);

simsignal_t EARA::LOOP_DETECTION_SIGNAL = SIMSIGNAL_NULL;
simsignal_t EARA::DROP_PACKET_WITH_ZERO_TTL = SIMSIGNAL_NULL;
simsignal_t EARA::ROUTE_FAILURE_NO_HOP = SIMSIGNAL_NULL;
simsignal_t EARA::NEW_ROUTE_DISCOVERY = SIMSIGNAL_NULL;
simsignal_t EARA::ROUTE_FAILURE_NEXT_HOP_IS_SENDER = SIMSIGNAL_NULL;

EARA::~EARA() {
    /* We set the policies to nullptr in order to prevent the AbstractARAClient from deleting those.
     * This is necessary because the surround omnetpp simulation will attempt to delete those modules
     * because they are SimpleModules which are owned by other compound modules*/

    AbstractARAClient::forwardingPolicy = nullptr;
    forwardingPolicy = nullptr;
    evaporationPolicy = nullptr;
    pathReinforcementPolicy = nullptr;
}

int EARA::numInitStages() const {
    return 5;
}

void EARA::initialize(int stage) {
    AbstractOMNeTARAClient::initialize(stage);
    if(stage == 4) {
        OMNeTEARAConfiguration config = OMNeTEARAConfiguration(this);
        setLogger(config.getLogger());

        AbstractEARAClient::initializeEARA(config);
        initializeNetworkInterfacesOf(this, config);

        notificationBoard->subscribe(this, NF_BATTERY_CHANGED);
        WATCH(nrOfDetectedLoops);
        WATCH(maximumBatteryCapacityInNetwork);
        LOOP_DETECTION_SIGNAL = registerSignal("routingLoopDetected");
        DROP_PACKET_WITH_ZERO_TTL = registerSignal("dropZeroTTLPacket");
        ROUTE_FAILURE_NO_HOP = registerSignal("routeFailureNoHopAvailable");
        NEW_ROUTE_DISCOVERY = registerSignal("newRouteDiscovery");
        ROUTE_FAILURE_NEXT_HOP_IS_SENDER =  registerSignal("routeFailureNextHopIsSender");
        new EARARoutingTableWatcher(routingTable);
    }
}

void EARA::receivePacket(Packet* packet, NetworkInterface* interface) {
    AbstractEARAClient::receivePacket(packet, interface);
    persistRoutingTableData();
}

void EARA::handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface) {
    AbstractARAClient::handleDuplicateErrorPacket(packet, interface);
    nrOfDetectedLoops++;
    emit(LOOP_DETECTION_SIGNAL, 1);
}

bool EARA::handleBrokenOMNeTLink(Packet* packet, AddressPtr receiverAddress, NetworkInterface* interface) {
    return AbstractEARAClient::handleBrokenLink(packet, receiverAddress, interface);
}

void EARA::handlePacketWithZeroTTL(Packet* packet) {
    if(packet->isDataPacket()) {
        emit(DROP_PACKET_WITH_ZERO_TTL, 1);
    }

    AbstractEARAClient::handlePacketWithZeroTTL(packet);
}

void EARA::handleNonSourceRouteDiscovery(Packet* packet) {
    if(routingTable->isDeliverable(packet->getDestination())) {
        // can not be sent because the only known next hop is the sender of this packet
        emit(ROUTE_FAILURE_NEXT_HOP_IS_SENDER, 1);
    }
    else {
        // can not be sent because there really is no known next hop
        emit(ROUTE_FAILURE_NO_HOP, 1);
    }
    AbstractEARAClient::handleNonSourceRouteDiscovery(packet);
}

void EARA::startNewRouteDiscovery(Packet* packet) {
    emit(NEW_ROUTE_DISCOVERY, 1);
    AbstractEARAClient::startNewRouteDiscovery(packet);
}

unsigned int EARA::getCurrentEnergyLevel() {
    return currentEnergyLevel;
}

OMNETARA_NAMESPACE_END
