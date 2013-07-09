/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "CLibs.h"
#include "TestbedARAClient.h"
#include "PacketDispatcher.h"
#include "BasicConfiguration.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClient::TestbedARAClient(Configuration& config) : AbstractARAClient(config){
    //TODO Make configurable
    this->logDebug("initialized testbedARAClient");
    initializeNetworkInterfaces();
    this->logDebug("initialized testbedARAClient network Interfaces");
}

TestbedARAClient::~TestbedARAClient() { }

void TestbedARAClient::sendPacket(Packet* packet) {
    this->logDebug("is sending packet # %u", packet->getSequenceNumber());
    if(routingTable->isDeliverable(packet)) {
        NextHop* nextHop = forwardingPolicy->getNextHop(packet, routingTable);
        nextHop->getInterface()->send(packet, nextHop->getAddress());
    }
    this->logDebug("sending FANT");
    this->sendFANT(packet->getDestination());
}

void TestbedARAClient::receivePacket(Packet* packet, ARA::NetworkInterface* interface) {
    this->logDebug("receiving packet # %u over interface at %s", packet->getSequenceNumber(), interface->getLocalAddress()->toString().c_str());
    AbstractARAClient::receivePacket(packet, interface);
    //TODO: persistRoutingTableData
}

void TestbedARAClient::deliverToSystem(const Packet* packet) {
    this->logDebug("sending packet # %u to System via TAP", packet->getSequenceNumber());
    dessert_syssend_msg(extractDessertMessage(packet));
}

void TestbedARAClient::packetNotDeliverable(const Packet* packet) {
    this->logDebug("packet # %u is undeliverable", packet->getSequenceNumber());
    delete packet;
    //nrOfNotDeliverablePackets++;
}

void TestbedARAClient::initializeNetworkInterfaces() {
    dessert_meshif_t* dessertInterfaces = dessert_meshiflist_get();
    while(dessertInterfaces != NULL) {
        NetworkInterface* newInterface = new NetworkInterface(dessertInterfaces, this, packetFactory, 400);
        addNetworkInterface(newInterface);
        dessertInterfaces = dessertInterfaces->next;
    }
}

TESTBED_NAMESPACE_END
