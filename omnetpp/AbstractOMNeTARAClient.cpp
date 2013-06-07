/*
 * $FU-Copyright$
 */

#include "omnetpp/AbstractOMNeTARAClient.h"
#include "omnetpp/TrafficControllInfo.h"
#include "omnetpp/RoutingTableWatcher.h"
#include "omnetpp/OMNeTGate.h"
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

AbstractOMNeTARAClient::~AbstractOMNeTARAClient() {
    DELETE_IF_NOT_NULL(routingTablePersistor);
    DELETE_IF_NOT_NULL(mobilityDataPersistor);
}

void AbstractOMNeTARAClient::initialize() {
    notificationBoard = NotificationBoardAccess().get();
    notificationBoard->subscribe(this, NF_LINK_BREAK);
    mobility = ModuleAccess<MobilityBase>("mobility").get();
    interfaceTable = ModuleAccess<IInterfaceTable>("interfaceTable").get();
    networkConfig = check_and_cast<ARANetworkConfigurator*>(simulation.getModuleByPath("networkConfigurator"));
    setPositionFromParameters();

    PACKET_DELIVERED_SIGNAL = registerSignal("packetDelivered");
    PACKET_NOT_DELIVERED_SIGNAL = registerSignal("packetUnDeliverable");
    ROUTE_FAILURE_SIGNAL = registerSignal("routeFailure");

    WATCH(nrOfDeliverablePackets);
    WATCH(nrOfNotDeliverablePackets);

    if(par("activateMobileTrace").boolValue()){
        mobilityDataPersistor = new MobilityDataPersistor(mobility, findHost());
    }

    routingTablePersistor = new RoutingTableDataPersistor(findHost(), par("routingTableStatisticsUpdate").longValue());
    new RoutingTableWatcher(routingTable);
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
    if(isFromUpperLayer(message)) {
        handleUpperLayerMessage(message);
    }
    else {
        handleARAMessage(message);
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

    OMNeTPacket* omnetPacket = (OMNeTPacket*) packetFactory->makeDataPacket(source, destination, sequenceNumber, payload, payloadSize);
    omnetPacket->encapsulate(check_and_cast<cPacket*>(message));

    sendPacket(omnetPacket);
}

void AbstractOMNeTARAClient::handleARAMessage(cMessage* message) {
      OMNeTPacket* omnetPacket = check_and_cast<OMNeTPacket*>(message);
      ASSERT(omnetPacket->getTTL() > 0);
      OMNeTGate* arrivalGate = (OMNeTGate*) getNetworkInterface(message->getArrivalGate()->getIndex());
      arrivalGate->receive(omnetPacket);
}

void AbstractOMNeTARAClient::receivePacket(Packet* packet, NetworkInterface* interface) {
    AbstractNetworkClient::receivePacket(packet, interface);
    routingTablePersistor->write(routingTable);
}

void AbstractOMNeTARAClient::takeAndSend(cMessage* msg, cGate* gate, double sendDelay) {
    Enter_Method_Silent("takeAndSend(msg)");
    take(msg);
    sendDelayed(msg, sendDelay, gate);
}

void AbstractOMNeTARAClient::deliverToSystem(const Packet* packet) {
    Packet* pckt = const_cast<Packet*>(packet); // we need to cast away the constness because the OMNeT++ method decapsulate() is not declared as const
    OMNeTPacket* omnetPacket = dynamic_cast<OMNeTPacket*>(pckt);
    ASSERT(omnetPacket);

    cPacket* encapsulatedData = omnetPacket->decapsulate();
    ASSERT(encapsulatedData);

    TrafficControlInfo* controlInfo = new TrafficControlInfo();
    int maxTTL = packetFactory->getMaximumNrOfHops();
    controlInfo->setHopCount(maxTTL - omnetPacket->getTTL());
    encapsulatedData->setControlInfo(controlInfo);

    send(encapsulatedData, "upperLayerGate$o");

    nrOfDeliverablePackets++;
    emit(PACKET_DELIVERED_SIGNAL, 1);

    delete omnetPacket;
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

        if(dynamic_cast<OMNeTPacket*>(encapsulatedPacket) != NULL) {
            // extract the receiver address
            MACAddress receiverMACAddress = frame->getReceiverAddress();
            IPv4Address receiverIPv4Address = networkConfig->getIPAddressFromMAC(receiverMACAddress);
            AddressPtr omnetAddress (new OMNeTAddress(receiverIPv4Address));

            OMNeTPacket* omnetPacket = check_and_cast<OMNeTPacket*>(encapsulatedPacket);

            // TODO this does only work if we have only one network interface card
            NetworkInterface* interface = getNetworkInterface(0);
            if (handleBrokenOMNeTLink(omnetPacket, omnetAddress, interface)) {
                emit(ROUTE_FAILURE_SIGNAL, 1);
            }
        }
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
        throw cRuntimeError("The TrafficGenerator could not determine the nodes interface");
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
}

OMNETARA_NAMESPACE_END
