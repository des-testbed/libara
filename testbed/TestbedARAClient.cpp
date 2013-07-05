/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "TestbedARAClient.h"
#include "PacketDispatcher.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClient::~TestbedARAClient() {

}

void TestbedARAClient::sendPacket(Packet* packet) {
    if(routingTable->isDeliverable(packet)) {
        NextHop* nextHop = forwardingPolicy->getNextHop(packet, routingTable);
        nextHop->getInterface()->send(packet, nextHop->getAddress());
    }
    this->sendFANT(packet->getDestination());
}

void TestbedARAClient::receivePacket(Packet* packet, ARA::NetworkInterface* interface) {
    AbstractARAClient::receivePacket(packet, interface);
    //TODO: persistRoutingTableData
}

void TestbedARAClient::deliverToSystem(const Packet* packet) {
    dessert_syssend_msg(extractDessertMessage(packet));
}

void TestbedARAClient::packetNotDeliverable(const Packet* packet) {
    delete packet;
    //nrOfNotDeliverablePackets++;
}

bool TestbedARAClient::handleBrokenLink(Packet* packet, AddressPtr nextHop, ARA::NetworkInterface* interface) {
    return AbstractARAClient::handleBrokenLink(packet, nextHop, interface);
}

void TestbedARAClient::timerHasExpired(Timer* responsibleTimer) {
    AbstractARAClient::timerHasExpired(responsibleTimer);
}

void TestbedARAClient::handlePacket(Packet* packet, ARA::NetworkInterface* interface) {
    AbstractARAClient::handlePacket(packet, interface);
}

void TestbedARAClient::handleDuplicateErrorPacket(Packet* packet, ARA::NetworkInterface* interface) {
    AbstractARAClient::handleDuplicateErrorPacket(packet, interface);
    //nrOfDetectedLoops++;
}

void TestbedARAClient::startNewRouteDiscovery(Packet* packet) {
    AbstractARAClient::startNewRouteDiscovery(packet);
}

void TestbedARAClient::handleNonSourceRouteDiscovery(Packet* packet) {
    AbstractARAClient::handleNonSourceRouteDiscovery(packet);
}

void TestbedARAClient::handlePacketWithZeroTTL(Packet* packet) {
    AbstractARAClient::handlePacketWithZeroTTL(packet);
}

TESTBED_NAMESPACE_END
