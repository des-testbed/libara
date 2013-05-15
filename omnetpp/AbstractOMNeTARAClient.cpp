/*
 * $FU-Copyright$
 */

#include "omnetpp/AbstractOMNeTARAClient.h"
#include "omnetpp/OMNeTGate.h"
#include "Environment.h"
#include "Ieee80211Frame_m.h"
#include "ModuleAccess.h"
#include "ARP.h"
#include "MACAddress.h"
#include "IPv4Address.h"

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
    send(encapsulatedData, "upperLayerGate$o");
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

OMNETARA_NAMESPACE_END
