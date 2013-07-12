/*
 * $FU-Copyright$
 */

#include "omnetpp/AbstractOMNeTARAClient.h"
#include "omnetpp/TrafficControllInfo.h"
#include "omnetpp/OMNeTGate.h"
#include "omnetpp/traffic/TrafficPacket_m.h"
#include "omnetpp/OMNeTPacket.h"
#include "omnetpp/OMNeTBattery.h"
#include "Environment.h"

#include "Ieee80211Frame_m.h"
#include "ModuleAccess.h"
#include "IPv4Address.h"
#include "IPv4InterfaceData.h"

#include <cstring>

OMNETARA_NAMESPACE_BEGIN

simsignal_t AbstractOMNeTARAClient::PACKET_DELIVERED_SIGNAL = SIMSIGNAL_NULL;
simsignal_t AbstractOMNeTARAClient::PACKET_NOT_DELIVERED_SIGNAL = SIMSIGNAL_NULL;
simsignal_t AbstractOMNeTARAClient::ROUTE_FAILURE_SIGNAL = SIMSIGNAL_NULL;
simsignal_t AbstractOMNeTARAClient::DROP_PACKET_BECAUSE_ENERGY_DEPLETED = SIMSIGNAL_NULL;

AbstractOMNeTARAClient::~AbstractOMNeTARAClient() {
    DELETE_IF_NOT_NULL(routingTablePersistor);
    DELETE_IF_NOT_NULL(mobilityDataPersistor);
}

void AbstractOMNeTARAClient::initialize(int stage) {
    if(stage == 0) {
        notificationBoard = NotificationBoardAccess().get();
        notificationBoard->subscribe(this, NF_LINK_BREAK);
        mobility = ModuleAccess<MobilityBase>("mobility").get();
        interfaceTable = ModuleAccess<IInterfaceTable>("interfaceTable").get();
        networkConfig = check_and_cast<ARANetworkConfigurator*>(simulation.getModuleByPath("networkConfigurator"));
        setPositionFromParameters();

        PACKET_DELIVERED_SIGNAL = registerSignal("packetDelivered");
        PACKET_NOT_DELIVERED_SIGNAL = registerSignal("packetUnDeliverable");
        ROUTE_FAILURE_SIGNAL = registerSignal("routeFailure");
        DROP_PACKET_BECAUSE_ENERGY_DEPLETED =  registerSignal("dropPacketBecauseEnergyDepleted");

        WATCH(currentEnergyLevel);
        WATCH(nrOfDeliverablePackets);
        WATCH(nrOfNotDeliverablePackets);

        if(par("activateMobileTrace").boolValue()){
            mobilityDataPersistor = new MobilityDataPersistor(mobility, findHost());
        }

        OMNeTBattery* battery = ModuleAccess<OMNeTBattery>("battery").get();
        currentEnergyLevel = battery->getCapacity();
        routingTablePersistor = new RoutingTableDataPersistor(findHost(), par("routingTableStatisticsUpdate").longValue());
    }
}

void AbstractOMNeTARAClient::setPositionFromParameters() {
    cModule* hostModule = findHost();

    const char* posX = hostModule->par("posX").stringValue();
    const char* posY = hostModule->par("posY").stringValue();

    if(std::strcmp(posX, "auto") != 0) {
        long x = getNewNodePosition(posX, mobility->par("constraintAreaMaxX").longValue(), mobility->par("constraintAreaMinX").longValue());
        hostModule->getDisplayString().setTagArg("p", 0, x);
    }

    if(std::strcmp(posY, "auto") != 0) {
        long y = getNewNodePosition(posY, mobility->par("constraintAreaMaxY").longValue(), mobility->par("constraintAreaMinY").longValue());
        hostModule->getDisplayString().setTagArg("p", 1, y);
    }
}

int AbstractOMNeTARAClient::getNewNodePosition(const char* positionParameter, int maxPosition, int minPosition) {
    int margin = 80;

    long newPosition;
    if(std::strcmp(positionParameter, "center") == 0) {
        newPosition = (maxPosition - minPosition) / 2;
    }
    else if(std::strcmp(positionParameter, "left") == 0 || std::strcmp(positionParameter, "top") == 0) {
        newPosition = minPosition + margin;
    }
    else if(std::strcmp(positionParameter, "right") == 0 || std::strcmp(positionParameter, "bottom") == 0) {
        newPosition = maxPosition - margin;
    }
    else {
        newPosition = atoi(positionParameter);
    }

    return newPosition;
}

void AbstractOMNeTARAClient::initializeNetworkInterfacesOf(AbstractARAClient* client, OMNeTConfiguration& config) {
    ASSERT(interfaceTable);
    int nrOfInterfaces = interfaceTable->getNumInterfaces();
    if(nrOfInterfaces > 2) { // loopback + 1 other NIC
        // TODO remove this constraint
        throw cRuntimeError("ARA does currently not implement handling of more than one network card.");
    }

    int nrOfAssignedInterfaces = 0;
    for (int i=0; i < nrOfInterfaces; i++)         {
        InterfaceEntry* interfaceEntry = interfaceTable->getInterface(i);
        if (interfaceEntry->isLoopback() == false) {
            client->addNetworkInterface(new OMNeTGate(this, client, gate("ifOut", nrOfAssignedInterfaces), interfaceEntry));
            nrOfAssignedInterfaces++;
        }
    }
}

void AbstractOMNeTARAClient::handleMessage(cMessage* message) {
    if (hasEnoughBattery) {
        if(isFromUpperLayer(message)) {
            handleUpperLayerMessage(message);
        }
        else {
            handleARAMessage(message);
        }
    }
    else {
        emit(DROP_PACKET_BECAUSE_ENERGY_DEPLETED, 1);
        delete message;
    }
}

bool AbstractOMNeTARAClient::isFromUpperLayer(cMessage* message) {
    std::string nameOfUpperLayergate = "upperLayerGate$i";
    std::string gateName = std::string(message->getArrivalGate()->getName());
    return gateName.length() <= nameOfUpperLayergate.length()
        && std::equal(gateName.begin(), gateName.end(), nameOfUpperLayergate.begin());
}

void AbstractOMNeTARAClient::handleUpperLayerMessage(cMessage* message) {
    TrafficControlInfo* controlInfo = check_and_cast<TrafficControlInfo*>(message->getControlInfo());
    AddressPtr destination = controlInfo->getDestination();
    AddressPtr source = getLocalAddress();
    unsigned int sequenceNumber = getNextSequenceNumber();

    // note that we implement the payload via encapsulation in OMNeT++
    const char* payload = nullptr;
    unsigned int payloadSize = 0;

    Packet* packet = packetFactory->makeDataPacket(source, destination, sequenceNumber, payload, payloadSize);
    cPacket* simPacket = dynamic_cast<cPacket*>(packet);
    ASSERT2(simPacket, "Model error: Something is wrong with the PacketFactory. It does not create cPacket compatible packets..");

    simPacket->encapsulate(check_and_cast<cPacket*>(message));
    sendPacket(packet);
}

void AbstractOMNeTARAClient::handleARAMessage(cMessage* message) {
    Packet* packet = dynamic_cast<Packet*>(message);
    ASSERT2(packet, "Model error: AbstractOMNeTARAClient tried to handle ARA message but can not cast to Packet*..");
    ASSERT(packet->getTTL() > 0);

    OMNeTGate* arrivalGate = (OMNeTGate*) getNetworkInterface(message->getArrivalGate()->getIndex());
    arrivalGate->receive(packet);
}

void AbstractOMNeTARAClient::persistRoutingTableData() {
    routingTablePersistor->write(routingTable);
}

void AbstractOMNeTARAClient::takeAndSend(cMessage* message, cGate* gate, double sendDelay) {
    Enter_Method_Silent("AbstractOMNeTARAClient::takeAndSend(...)");
    take(message);

    updatePacketRouteStatistics(message);
    sendDelayed(message, sendDelay, gate);
}

void AbstractOMNeTARAClient::updatePacketRouteStatistics(cMessage* msg) {
    Packet* araPacket = check_and_cast<Packet*>(msg);
    cPacket* simPacket = check_and_cast<cPacket*>(msg);
    if (araPacket->isDataPacket()) {
        // store the route and energy along that route for statistics later (not used in the routing algorithm itself)
        TrafficPacket* encapsulatedPacket = check_and_cast<TrafficPacket*>(simPacket->getEncapsulatedPacket());
        Route route = encapsulatedPacket->getRoute();
        route.push_back(getLocalAddress());

        int oldRouteEnergy = encapsulatedPacket->getRouteEnergy();
        encapsulatedPacket->setRouteEnergy(oldRouteEnergy + currentEnergyLevel);
    }
}

void AbstractOMNeTARAClient::deliverToSystem(const Packet* packet) {
    Packet* pckt = const_cast<Packet*>(packet); // we need to cast away the constness because the OMNeT++ method decapsulate() is not declared as const
    cPacket* simPacket = dynamic_cast<cPacket*>(pckt);
    ASSERT2(simPacket, "Model error: AbstractOMNeTARAClient tried to deliver packet to system, but it can not cast to Packet*..");

    cPacket* encapsulatedData = simPacket->decapsulate();
    ASSERT(encapsulatedData);

    TrafficControlInfo* controlInfo = new TrafficControlInfo();
    int maxTTL = packetFactory->getMaximumNrOfHops();
    controlInfo->setHopCount(maxTTL - packet->getTTL());
    encapsulatedData->setControlInfo(controlInfo);

    send(encapsulatedData, "upperLayerGate$o");

    nrOfDeliverablePackets++;
    emit(PACKET_DELIVERED_SIGNAL, 1);

    delete packet;
}

void AbstractOMNeTARAClient::packetNotDeliverable(const Packet* packet) {
    delete packet;
    nrOfNotDeliverablePackets++;
    emit(PACKET_NOT_DELIVERED_SIGNAL, 1);
}

void AbstractOMNeTARAClient::receiveChangeNotification(int category, const cObject* details) {
    if(category == NF_LINK_BREAK) {
        Ieee80211DataOrMgmtFrame* frame = check_and_cast<Ieee80211DataOrMgmtFrame*>(details);
        cPacket* encapsulatedPacket = frame->decapsulate();

        if (dynamic_cast<Packet*>(encapsulatedPacket) != NULL) {
            // extract the receiver address
            MACAddress receiverMACAddress = frame->getReceiverAddress();
            IPv4Address receiverIPv4Address = networkConfig->getIPAddressFromMAC(receiverMACAddress);
            AddressPtr omnetAddress (new OMNeTAddress(receiverIPv4Address));

            Packet* packet = check_and_cast<Packet*>(encapsulatedPacket);

            // TODO this does only work if we have only one network interface card
            NetworkInterface* interface = getNetworkInterface(0);
            if (handleBrokenOMNeTLink(packet, omnetAddress, interface)) {
                emit(ROUTE_FAILURE_SIGNAL, 1);
            }
        }
    }
    else if(category == NF_BATTERY_CHANGED) {
        handleBatteryStatusChange(check_and_cast<Energy*>(details));
    }
}

void AbstractOMNeTARAClient::handleBatteryStatusChange(Energy* energyInformation) {
    currentEnergyLevel = energyInformation->GetEnergy();

    if (currentEnergyLevel <= 0) {
       hasEnoughBattery = false;
       nodeEnergyDepletionTimestamp = simTime();

       // change the node color
       cDisplayString& displayString = getParentModule()->getParentModule()->getDisplayString();
       displayString.setTagArg("i", 1, "#FF0000");
    }
}

IInterfaceTable* AbstractOMNeTARAClient::getInterfaceTable() {
    return interfaceTable;
}

cModule* AbstractOMNeTARAClient::findHost() const {
    cModule* module;
    for (module = getParentModule(); module != 0; module = module->getParentModule()) {
        cProperties* properties = module->getProperties();
        if (properties && properties->getAsBool("node")) {
            return module;
        }
    }

    throw cRuntimeError("Could not determine the host module (should have a property named @node)");
}

AddressPtr AbstractOMNeTARAClient::getLocalAddress() {
    //TODO this does currently only support one interface
    InterfaceEntry* interface = nullptr;
    int nrOfInterfaces = interfaceTable->getNumInterfaces();
    for (int k=0; k < nrOfInterfaces; k++) {
        InterfaceEntry* currentInterfaceEntry = interfaceTable->getInterface(k);
        if(currentInterfaceEntry->isLoopback() == false) {
            if(interface != nullptr) {
                throw cRuntimeError("The AbstractOMNeTARAClient does currently only support one single interface per node");
            }
            else {
                interface = currentInterfaceEntry;
            }
        }
    }

    if(interface == nullptr) {
        throw cRuntimeError("Could not determine the nodes interface");
    }

    IPv4Address ipv4 = interface->ipv4Data()->getIPAddress();
    return AddressPtr(new OMNeTAddress(ipv4));
}

void AbstractOMNeTARAClient::finish() {
    recordScalar("nrOfTrappedPacketsAfterFinish", packetTrap->getNumberOfTrappedPackets());

    int64 nrOfSentDataBits = 0;
    int64 nrOfSentControlBits = 0;
    unsigned int nrOfControlPackets = 0;
    unsigned int nrOfDataPackets = 0;
    for(auto& interface: interfaces) {
        OMNeTGate* gate = (OMNeTGate*) interface;
        nrOfSentDataBits += gate->getNrOfSentDataBits();
        nrOfSentControlBits += gate->getNrOfSentControlBits();
        nrOfControlPackets += gate->getNrOfControlPackets();
        nrOfDataPackets += gate->getNrOfDataPackets();
    }

    recordScalar("nrOfSentDataBits", nrOfSentDataBits);
    recordScalar("nrOfSentControlBits", nrOfSentControlBits);
    recordScalar("nrOfControlPackets", nrOfControlPackets);
    recordScalar("nrOfDataPackets", nrOfDataPackets);

    if (nodeEnergyDepletionTimestamp > 0) {
        recordScalar("nodeEnergyDepletionTimestamp", nodeEnergyDepletionTimestamp);
    }
}

OMNETARA_NAMESPACE_END
