/*
 * $FU-Copyright$
 */

#include "TestbedPacket.h"
#include "TestbedPacketDispatcher.h"
#include "TestbedAddress.h"

#include <netinet/in.h>

extern std::shared_ptr<ARA::testbed::TestbedARAClient> client;

TESTBED_NAMESPACE_BEGIN

/**
 * RECEIVE
 */
dessert_cb_result toSys(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags, dessert_meshif_t* interface, dessert_frameid_t id) {
    TestbedPacketFactory* packetFactory = dynamic_cast<TestbedPacketFactory*>(client->getPacketFactory());
    /// set the time to live
    message->ttl = packetFactory->getMaximumNrOfHops();
    /// add a sequence number (TODO: this might become a problem)
    message->u16 = client->getNextSequenceNumber();
    /// set the flags 
    //ap->flags |= ARA_ORIG_LOCAL;

    /* add trace header if packet is small broadcast/multicast packet
       this helps to abuse such packets for better loop/duplicate detection */
//    struct ether_header* ethernetHeader = nullptr;
//    assert(dessert_msg_getpayload(message, (void**) &ethernetHeader));

    /// TODO: 512 refers to ara_trace_broadcastlen from DES-ARA
 //   if((ethernetHeader->ether_dhost[0] & 0x01) && length < 512) {
 //       dessert_msg_trace_initiate(message, DESSERT_EXT_TRACE_REQ, DESSERT_MSG_TRACE_HOST);
 //   }

    // DEBUG: 
    std::cerr << "[toSys] got a packet" << std::endl;
    TestbedPacket* packet = packetFactory->makeNewPacket(message);
    // DEBUG: 
    std::cerr << "[toSys] packet dump " << std::endl;
    // DEBUG: 
    std::cerr << "received frame " << ((unsigned long)id) << " on interface " << interface->if_name << std::endl;
    // DEBUG:
    dumpDessertMessage(message, length, flags);
    TestbedNetworkInterface* networkInterface = client->getTestbedNetworkInterface(std::make_shared<TestbedAddress>(interface->hwaddr));
    // DEBUG:
    std::cerr << "[toSys] will pass packet to network interface (receive)" << std::endl;

    networkInterface->receive(packet);

    return DESSERT_MSG_DROP;
}

/**
 * SEND
 */
int dispatch(const Packet* packet, std::shared_ptr<TestbedAddress> interfaceAddress, std::shared_ptr<Address> recipient) {
    assert(packet != nullptr);

    /// determine the mesh interface
    dessert_meshif_t* interface = dessert_meshif_get_hwaddr(interfaceAddress->getDessertValue());
    // DEBUG: 
    std::cerr << "[TestbedPacketDispatcher::dispatch] ready to transmit a packet over interface " << interface->if_name << std::endl;

    const TestbedPacket* testbedPacket = dynamic_cast<const TestbedPacket*>(packet);
    // create a dessert message out of the packet
    dessert_msg_t* message = testbedPacket->toDessertMessage();

    /// send the packet
    int result = 42;

    if ((result = dessert_meshsend(message, interface)) == DESSERT_OK) {
        // DEBUG: 
        std::cerr << "[TestbedPacketDispatcher::dispatch] sending message was successful" << std::endl;
    } else if (result == EINVAL) {
        // DEBUG: 
        std::cerr << "[TestbedPacketDispatcher::dispatch] message was broken" << std::endl;
    } else if (result == EIO) {
        // DEBUG: 
        std::cerr << "[TestbedPacketDispatcher::dispatch] message was not sent succesfully" << std::endl;
    } else {
        // DEBUG: 
        std::cerr << "[TestbedPacketDispatcher::dispatch] unknown error code" << std::endl;
    }

    return DESSERT_MSG_DROP;
}



dessert_cb_result toMesh(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags, dessert_sysif_t *interface, dessert_frameid_t id) {
    // DEBUG: 
    std::cerr << "[toMesh] received frame " << ((unsigned long)id) << " on interface " << interface->if_name << std::endl;
    // DEBUG: 
    dumpDessertMessage(message, length, flags);

    TestbedPacketFactory* packetFactory = dynamic_cast<TestbedPacketFactory*>(client->getPacketFactory());
    // DEBUG: 
    std::cerr << "[toMesh] got a packet" << std::endl;
    TestbedPacket* packet = packetFactory->makeNewPacket(message);
    // DEBUG: 
    std::cerr << "[toMesh] packet dump:" << std::endl;
    // DEBUG: 
    std::cerr << *packet << std::endl;
    // DEBUG:
    std::cerr << "[toMesh] pass packet to client" << std::endl;

    client->sendPacket(packet);

    return DESSERT_MSG_DROP;
}

void dumpDessertMessage(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags) {
    char buffer[1024];
    dessert_msg_proc_dump(message, length, flags, buffer, 1024);
    std::cerr << buffer;
    std::cerr << std::endl;
}

dessert_cb_result packetFilter(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags, dessert_sysif_t *interface, dessert_frameid_t id) {
    dessert_ext_t* extension;
    dessert_msg_getext(message, &extension, DESSERT_EXT_ETH, 0);
    ether_header* ethernetFrame = (ether_header*) extension->data;

    if(ethernetFrame->ether_dhost[0] == 51) {
        return DESSERT_MSG_DROP;
    } else {
        return DESSERT_MSG_KEEP;
    }
}

TESTBED_NAMESPACE_END
