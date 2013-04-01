/*
 * $FU-Copyright$
 */

#include "omnetpp/EARA.h"

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
        OMNeTConfiguration config = OMNeTConfiguration(this);
        setLogger(config.getLogger());

        AbstractEARAClient::initialize(config, config.getRoutingTable());
        initializeNetworkInterfacesOf(this, config);

        //FIXME subscribe for the battery event!!
        notificationBoard->subscribe(this, NF_LINK_BREAK);
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
    if (energyInformation->GetEnergy() <= 0) {
       hasEnoughBattery = false;

       // change the node color
       cDisplayString& displayString = getParentModule()->getParentModule()->getDisplayString();
       displayString.setTagArg("i", 1, "#FF0000");
    }
}

unsigned char EARA::getCurrentEnergyLevel() {
    //FIXME implement this
    return 255;
}

OMNETARA_NAMESPACE_END
