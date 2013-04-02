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

OMNETARA_NAMESPACE_BEGIN

bool AbstractOMNeTARAClient::isEnvironmentInitialized = false;

void AbstractOMNeTARAClient::initialize() {
    initializeEnvironment();
    notificationBoard = NotificationBoardAccess().get();
    notificationBoard->subscribe(this, NF_LINK_BREAK);
    interfaceTable = ModuleAccess<IInterfaceTable>("interfaceTable").get();
}

void AbstractOMNeTARAClient::initializeEnvironment() {
    if(isEnvironmentInitialized == false) {
        // The clock is initialized directly in the OMNeTClock
        Environment::setPacketFactory(new ::ARA::omnetpp::PacketFactory());
        isEnvironmentInitialized = true;
    }
}

void AbstractOMNeTARAClient::initializeNetworkInterfacesOf(AbstractARAClient* client, OMNeTConfiguration& config) {
    cGate* gateToARP = gate("arpOut");

    ASSERT(interfaceTable);
    int nrOfInterfaces = interfaceTable->getNumInterfaces();
    if(nrOfInterfaces > 2) { // loopback + 1 other NIC
        // TODO remove this constraint
        throw cRuntimeError("ARA does currently not implement handling of more than one network card.");
    }

    for (int i=0; i < nrOfInterfaces; i++)         {
        InterfaceEntry* interfaceEntry = interfaceTable->getInterface(i);
        if (interfaceEntry->isLoopback() == false) {
            client->addNetworkInterface(new OMNeTGate(this, client, gateToARP, interfaceEntry, config.getBroadCastDelay(), config.getUniCastDelay()));
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
            ARP* arp = ModuleAccess<ARP>("arp").get();
            MACAddress receiverMACAddress = frame->getReceiverAddress();
            const IPv4Address receiverIPv4Address = arp->getInverseAddressResolution(receiverMACAddress);
            AddressPtr omnetAddress (new OMNeTAddress(receiverIPv4Address));

            OMNeTPacket* omnetPacket = check_and_cast<OMNeTPacket*>(encapsulatedPacket);
            handleBrokenLink(omnetPacket, omnetAddress);
        }
    }
}

IInterfaceTable* AbstractOMNeTARAClient::getInterfaceTable() {
    return interfaceTable;
}

OMNETARA_NAMESPACE_END
