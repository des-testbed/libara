/*
 * $FU-Copyright$
 */

#include "omnetpp/EARA.h"
#include "omnetpp/OMNeTEARAConfiguration.h"
#include "omnetpp/PacketFactory.h"

OMNETARA_NAMESPACE_BEGIN

// Register the class with the OMNeT++ simulation
Define_Module(EARA);

EARA::EARA() {
    messageDispatcher = new MessageDispatcher(this, this);
}

EARA::~EARA() {
    delete messageDispatcher;
}

int EARA::numInitStages() const {
    return 5;
}

void EARA::initialize(int stage) {
    if(stage == 4) {
        AbstractOMNeTARAClient::initialize();
        OMNeTEARAConfiguration config = OMNeTEARAConfiguration(this);
        setLogger(config.getLogger());
        packetFactory = new PacketFactory(config.getMaxTTL());

        AbstractEARAClient::initializeEARA(config, config.getRoutingTable(), packetFactory);
        initializeNetworkInterfacesOf(this, config);

        notificationBoard->subscribe(this, NF_BATTERY_CHANGED);
        maximumBatteryLevel = config.getMaximumBatteryLevel();
        currentEnergyLevel =  255;
        WATCH(currentEnergyLevel);
    }
}

void EARA::handleMessage(cMessage* message) {
    if (hasEnoughBattery) {
        messageDispatcher->dispatch(message);
    }
}

void EARA::deliverToSystem(const Packet* packet) {
    sendToUpperLayer(packet);
}

void EARA::packetNotDeliverable(const Packet* packet) {
    //TODO report to upper layer
}

void EARA::receiveChangeNotification(int category, const cObject* details) {
    if(category == NF_BATTERY_CHANGED) {
        handleBatteryStatusChange(check_and_cast<Energy*>(details));
    }
    else {
        AbstractOMNeTARAClient::receiveChangeNotification(category, details);
    }
}

void EARA::handleBrokenLink(OMNeTPacket* packet, AddressPtr receiverAddress) {
    // TODO this does only work if we have only one network interface card
    NetworkInterface* interface = getNetworkInterface(0);
    handleRouteFailure(packet, receiverAddress, interface);
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
