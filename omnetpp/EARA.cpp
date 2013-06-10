/*
 * $FU-Copyright$
 */

#include "omnetpp/EARA.h"
#include "omnetpp/OMNeTEARAConfiguration.h"
#include "omnetpp/PacketFactory.h"

OMNETARA_NAMESPACE_BEGIN

// Register the class with the OMNeT++ simulation
Define_Module(EARA);

simsignal_t EARA::LOOP_DETECTION_SIGNAL = SIMSIGNAL_NULL;
simsignal_t EARA::DROP_PACKET_WITH_ZERO_TTL = SIMSIGNAL_NULL;
simsignal_t EARA::ROUTE_FAILURE_NO_HOP = SIMSIGNAL_NULL;
simsignal_t EARA::NEW_ROUTE_DISCOVERY = SIMSIGNAL_NULL;
simsignal_t EARA::ROUTE_FAILURE_NEXT_HOP_IS_SENDER = SIMSIGNAL_NULL;
simsignal_t EARA::DROP_PACKET_BECAUSE_ENERGY_DEPLETED = SIMSIGNAL_NULL;

EARA::~EARA() {
    /* We set the policies to nullptr in order to prevent the AbstractARAClient from deleting those.
     * This is necessary because the surround omnetpp simulation will attempt to delete those modules
     * because they are SimpleModules which are owned by other compound modules*/

    forwardingPolicy = nullptr;
    evaporationPolicy = nullptr;
    pathReinforcementPolicy = nullptr;
}

int EARA::numInitStages() const {
    return 5;
}

void EARA::initialize(int stage) {
    if(stage == 0) {
        AbstractOMNeTARAClient::initialize();
    }
    else if(stage == 4) {
        OMNeTEARAConfiguration config = OMNeTEARAConfiguration(this);
        setLogger(config.getLogger());
        PacketFactory* packetFactory = new PacketFactory(config.getMaxTTL());

        AbstractEARAClient::initializeEARA(config, config.getRoutingTable(), packetFactory);
        initializeNetworkInterfacesOf(this, config);

        notificationBoard->subscribe(this, NF_BATTERY_CHANGED);
        maximumBatteryLevel = config.getMaximumBatteryLevel();
        currentEnergyLevel =  255;
        WATCH(currentEnergyLevel);
        WATCH(nrOfDetectedLoops);
        LOOP_DETECTION_SIGNAL = registerSignal("routingLoopDetected");
        DROP_PACKET_WITH_ZERO_TTL = registerSignal("dropZeroTTLPacket");
        ROUTE_FAILURE_NO_HOP = registerSignal("routeFailureNoHopAvailable");
        NEW_ROUTE_DISCOVERY = registerSignal("newRouteDiscovery");
        ROUTE_FAILURE_NEXT_HOP_IS_SENDER =  registerSignal("routeFailureNextHopIsSender");
        DROP_PACKET_BECAUSE_ENERGY_DEPLETED =  registerSignal("dropPacketBecauseEnergyDepleted");
        energyLevelOutVector.setName("energyLevel");
    }
}

void EARA::handleMessage(cMessage* message) {
    energyLevelOutVector.record(getCurrentEnergyLevel());

    if (hasEnoughBattery) {
        AbstractOMNeTARAClient::handleMessage(message);
    }
    else {
        emit(DROP_PACKET_BECAUSE_ENERGY_DEPLETED, 1);
        delete message;
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

bool EARA::handleBrokenOMNeTLink(OMNeTPacket* packet, AddressPtr receiverAddress, NetworkInterface* interface) {
    return AbstractEARAClient::handleBrokenLink(packet, receiverAddress, interface);
}

void EARA::handlePacketWithZeroTTL(Packet* packet) {
    AbstractEARAClient::handlePacketWithZeroTTL(packet);

    if(packet->isDataPacket()) {
        emit(DROP_PACKET_WITH_ZERO_TTL, 1);
    }
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

void EARA::receiveChangeNotification(int category, const cObject* details) {
    if(category == NF_BATTERY_CHANGED) {
        handleBatteryStatusChange(check_and_cast<Energy*>(details));
    }
    else {
        AbstractOMNeTARAClient::receiveChangeNotification(category, details);
    }
}

void EARA::handleBatteryStatusChange(Energy* energyInformation) {
    currentEnergyLevel = (energyInformation->GetEnergy() / maximumBatteryLevel) * 255;

    if (currentEnergyLevel <= 0) {
       hasEnoughBattery = false;

       // change the node color
       cDisplayString& displayString = getParentModule()->getParentModule()->getDisplayString();
       displayString.setTagArg("i", 1, "#FF0000");
    }
}

unsigned char EARA::getCurrentEnergyLevel() {
    return currentEnergyLevel;
}

OMNETARA_NAMESPACE_END
