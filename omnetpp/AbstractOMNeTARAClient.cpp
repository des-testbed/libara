/*
 * $FU-Copyright$
 */

#include "omnetpp/AbstractOMNeTARAClient.h"
#include "omnetpp/TrafficControllInfo.h"
#include "omnetpp/OMNeTGate.h"
#include "Environment.h"

#include "Ieee80211Frame_m.h"
#include "ModuleAccess.h"
#include "IPv4Address.h"
#include "IPv4InterfaceData.h"

#include <cstring>

OMNETARA_NAMESPACE_BEGIN

void AbstractOMNeTARAClient::initialize() {
    notificationBoard = NotificationBoardAccess().get();
    notificationBoard->subscribe(this, NF_LINK_BREAK);
    mobility = ModuleAccess<MobilityBase>("mobility").get();
    interfaceTable = ModuleAccess<IInterfaceTable>("interfaceTable").get();
    networkConfig = check_and_cast<ARANetworkConfigurator*>(simulation.getModuleByPath("networkConfigurator"));
    setPositionFromParameters();
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

void AbstractOMNeTARAClient::takeAndSend(cMessage* msg, cGate* gate, double sendDelay) {
    Enter_Method_Silent("takeAndSend(msg)");
    take(msg);
    sendDelayed(msg, sendDelay, gate);
}

void AbstractOMNeTARAClient::sendToUpperLayer(const Packet* packet) {
    Packet* pckt = const_cast<Packet*>(packet); // we need to cast away the constness because the OMNeT++ method decapsulate() is not declared as const
    OMNeTPacket* omnetPacket = dynamic_cast<OMNeTPacket*>(pckt);
    ASSERT(omnetPacket);

    cPacket* encapsulatedData = omnetPacket->decapsulate();
    ASSERT(encapsulatedData);

    send(encapsulatedData, "upperLayerGate$o");
    delete omnetPacket;
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
            handleBrokenOMNeTLink(omnetPacket, omnetAddress);
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

OMNETARA_NAMESPACE_END
