/*
 * $FU-Copyright$
 */

#ifndef PACKET_DISPATCHER_H
#define PACKET_DISPATCHER_H

#include "CLibs.h"
#include "Testbed.h"
#include "Packet.h"
#include "testbed/NetworkInterface.h"
#include <unordered_map>

TESTBED_NAMESPACE_BEGIN

typedef std::unordered_map<dessert_meshif_t*, NetworkInterface*> NetworkInterfaceMap; //instantiated in .cpp file

struct routingExtension {
    u_int8_t  ara_dhost[ETH_ALEN];  /* ARA destination eth addr */
    u_int8_t  ara_shost[ETH_ALEN];  /* ARA source ether addr    */
    u_int16_t ara_data;             /* reserved for future ARA use */
  } __attribute__ ((__packed__));

/**
 * Receives a dessert_message_t from the NIC and sends to libARA.
 */
_dessert_cb_results messageFromMeshInterfaceDispatcher(dessert_msg_t* messageReceived, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_meshif_t* interface, dessert_frameid_t id);

/**
 * Determines if a given dessert message was created by TestbedARA.
 */
bool isARAMessage(dessert_msg_t* message);

/**
 * Receives a Packet from an Interface and converts to a message, then sends it over selected interface (if NULL sends over all interfaces)
 */
void packetToMeshInterfaceDispatcher(const Packet* packet, NetworkInterface* testbedInterface, std::shared_ptr<Address> recipient);

/**
 * Stub; tells DES-SERT to drop packets received from user space.
 * Note, libARA currently does not use the sys interface to inject data to DES-SERT
 */
_dessert_cb_results messageToMeshInterfaceDispatcher(dessert_msg_t* messageToSend, uint32_t length, dessert_msg_proc_t *processingFlags, dessert_sysif_t *interface, dessert_frameid_t id);

/**
 * Extracts all data from a dessert message that is necessary to create a libARA Packet object.
 */
Packet* extractPacket(dessert_msg_t* dessertMessage);

/**
 * Extracts the ethernet header struct from the given dessert message.
 */
ether_header* extractEthernetHeader(dessert_msg_t* dessertMessage);

/**
 * Extracts the LibARA routingExtension from the given dessert message.
 */
routingExtension* extractRoutingExtension(dessert_msg_t* dessertMessage);

/**
 * Extracts all data from a packet and stores in a dessert message.
 */
dessert_msg_t* extractDessertMessage(const Packet* packet);

/**
 * Adds a ethernet header to a dessert message.
 */
void addEthernetHeader(dessert_msg_t* message, std::shared_ptr<Address> nextHop);

/**
 * Returns a pointer to the testbed network interface paired with a given dessert meshinterface.
 */
NetworkInterface* extractNetworkInterface(dessert_meshif_t* dessertInterface);

TESTBED_NAMESPACE_END

#endif /*PACKET_DISPATCHER_H*/
