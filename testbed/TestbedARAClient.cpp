/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "CLibs.h"
#include "TestbedARAClient.h"
#include "PacketDispatcher.h"
#include "BasicConfiguration.h"
#include "SimpleLogger.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClient::TestbedARAClient(Configuration& config) : AbstractARAClient(config){
    //TODO Make configurable
    Logger* logger = new SimpleLogger("ara");
    setLogger(logger);
    logDebug("Initialized testbedARAClient");
    initializeNetworkInterfaces();
    logDebug("Initialized testbedARAClient network Interfaces");
}

TestbedARAClient::~TestbedARAClient() { }

void TestbedARAClient::sendPacket(Packet* packet) {
    logDebug("is sending packet # %u", packet->getSequenceNumber());
    if(routingTable->isDeliverable(packet)) {
        NextHop* nextHop = forwardingPolicy->getNextHop(packet, routingTable);
        nextHop->getInterface()->send(packet, nextHop->getAddress());
    }
    logDebug("sending FANT");
    std::cout << "sending FANT" << std::endl;
    sendFANT(packet->getDestination());
}

void TestbedARAClient::receivePacket(Packet* packet, ARA::NetworkInterface* interface) {
    logDebug("receiving packet # %u over interface at %s", packet->getSequenceNumber(), interface->getLocalAddress()->toString().c_str());
    AbstractARAClient::receivePacket(packet, interface);
    //TODO: persistRoutingTableData
}

void TestbedARAClient::deliverToSystem(const Packet* packet) {
    logDebug("sending packet # %u to System via TAP", packet->getSequenceNumber());
    dessert_syssend_msg(extractDessertMessage(packet));
}

void TestbedARAClient::packetNotDeliverable(const Packet* packet) {
    logDebug("packet # %u is undeliverable", packet->getSequenceNumber());
    delete packet;
    //nrOfNotDeliverablePackets++;
}

void TestbedARAClient::initializeNetworkInterfaces() {
    dessert_meshif_t* dessertInterfaces = dessert_meshiflist_get();
    while(dessertInterfaces != NULL) {
        NetworkInterface* newInterface = new NetworkInterface(dessertInterfaces, this, packetFactory, 400);
        addNetworkInterface(newInterface);
        logDebug("Initialized network interface: %s", dessertInterfaces->if_name);
        dessertInterfaces = dessertInterfaces->next;
    }
}

TESTBED_NAMESPACE_END
