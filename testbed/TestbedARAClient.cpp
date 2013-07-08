/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "TestbedARAClient.h"
#include "PacketDispatcher.h"
#include "BasicConfiguration.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClient::TestbedARAClient() {
    //TODO Make configurable
    Configuration* config = new BasicConfiguration(routingTable, packetFactory, evaporationPolicy, pathReinforcementPolicy, forwardingPolicy, 25.0, 5, 600, 5);
    initialize(*config);
    this->logDebug("initialized testBedARAClient");
}

TestbedARAClient::~TestbedARAClient() {
}

void TestbedARAClient::sendPacket(Packet* packet) {
    this->logDebug("is sending packet #"+packet->getSequenceNumber());
    if(routingTable->isDeliverable(packet)) {
        NextHop* nextHop = forwardingPolicy->getNextHop(packet, routingTable);
        nextHop->getInterface()->send(packet, nextHop->getAddress());
    }
    this->logDebug("sending FANT");
    this->sendFANT(packet->getDestination());
}

void TestbedARAClient::receivePacket(Packet* packet, ARA::NetworkInterface* interface) {
    this->logDebug("receiving packet #"+packet->getSequenceNumber()+" over interface at "+interface->getLocalAddress());
    AbstractARAClient::receivePacket(packet, interface);
    //TODO: persistRoutingTableData
}

void TestbedARAClient::deliverToSystem(const Packet* packet) {
    this->logDebug("sending packet #"+packet->getSequenceNumber()+" to System via TAP");
    dessert_syssend_msg(extractDessertMessage(packet));
}

void TestbedARAClient::packetNotDeliverable(const Packet* packet) {
    this->logDebug("packet #"+packet->getSequenceNumber()+" is undeliverable");
    delete packet;
    //nrOfNotDeliverablePackets++;
}

TESTBED_NAMESPACE_END
