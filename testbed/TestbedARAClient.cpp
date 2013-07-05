/*
 * $FU-Copyright$
 */

#include "Testbed.h"
#include "TestbedARAClient.h"

TESTBED_NAMESPACE_BEGIN

TestbedARAClient::~TestbedARAClient() {

}

void TestbedARAClient::sendPacket(Packet* packet) {

}

void TestbedARAClient::receivePacket(Packet* packet, NetworkInterface* interface) {

}

void TestbedARAClient::deliverToSystem(const Packet* packet) {

}

void TestbedARAClient::packetNotDeliverable(const Packet* packet) {

}

bool TestbedARAClient::handleBrokenLink(Packet* packet, AddressPtr nextHop, NetworkInterface* interface) {
    return false;
}

void TestbedARAClient::timerHasExpired(Timer* responsibleTimer) {

}

void TestbedARAClient::handlePacket(Packet* packet, NetworkInterface* interface) {

}

void TestbedARAClient::handleDuplicateErrorPacket(Packet* packet, NetworkInterface* interface) {

}

void TestbedARAClient::startNewRouteDiscovery(Packet* packet) {

}

void TestbedARAClient::handleNonSourceRouteDiscovery(Packet* packet) {

}

void TestbedARAClient::handlePacketWithZeroTTL(Packet* packet) {

}

TESTBED_NAMESPACE_END
