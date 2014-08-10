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
//    message->ttl = packetFactory->getMaximumNrOfHops();
    /// add a sequence number (TODO: this might become a problem)
//    message->u16 = client->getNextSequenceNumber();
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
 //
 //
    // DEBUG: std::cerr << "[toSys] got a packet" << std::endl;

    // let's check if the received message is consistent
    if (packetFactory->checkDessertMessage(message)) {
        TestbedPacket* packet = packetFactory->makeNewPacket(message);
        // DEBUG: 
        std::cerr << "[toSys] packet dump " << std::endl;
        // DEBUG: std::cerr << "received frame " << ((unsigned long)id) << " on interface " << interface->if_name << std::endl;
        
        // DEBUG: 
        dumpDessertMessage(message, length, flags);

        TestbedNetworkInterface* networkInterface = client->getTestbedNetworkInterface(std::make_shared<TestbedAddress>(interface->hwaddr));
        // DEBUG:
        std::cerr << "[toSys] will pass packet to network interface (receive)" << std::endl;

        networkInterface->receive(packet);
    }

    //return DESSERT_MSG_DROP;
    return DESSERT_MSG_KEEP;
}

/**
 * SEND
 */
int dispatch(const Packet* packet, std::shared_ptr<TestbedAddress> interfaceAddress, std::shared_ptr<Address> recipient) {
    assert(packet != nullptr);

    /// determine the mesh interface
    dessert_meshif_t* interface = dessert_meshif_get_hwaddr(interfaceAddress->getDessertValue());
    // DEBUG: std::cerr << "[TestbedPacketDispatcher::dispatch] ready to transmit a packet over interface " << interface->if_name << std::endl;

    const TestbedPacket* testbedPacket = dynamic_cast<const TestbedPacket*>(packet);
    // create a dessert message out of the packet
    dessert_msg_t* message = testbedPacket->toDessertMessage();

    // DEBUG:
    std::cout << "[toMesh] packet dump " <<  std::endl;
    std::cout << toString(message);

    /// send the packet
    int result = 42;

    TestbedAddressPtr recipientAddress = std::dynamic_pointer_cast<TestbedAddress>(recipient);

    /// FIXME: 
    ara_address_t tmp; 
    /// FIXME: backup the address
    memcpy(tmp, message->l2h.ether_dhost, sizeof(ara_address_t));
    /// set the next hop as the destination host 
    memcpy(message->l2h.ether_dhost, recipientAddress->getDessertValue(), sizeof(ara_address_t));

    // DEBUG:
    std::cerr << "[TestbedPacketDispatcher::dispatch] next hop is " << recipientAddress->toString() << std::endl;

    TestbedPacketFactory* packetFactory = dynamic_cast<TestbedPacketFactory*>(client->getPacketFactory());

    /**
     * We have to perform the message consistent check by ourselves. The
     * dessert_meshsend() function performs a check, but expects the header and
     * payload length to be in host byte order. That would be fine if the
     * function would transform the values afterwards in network byte order.
     * That appears to be a bug in libdessert (doesn't make that much sense
     * otherwise).
     */
    if (packetFactory->checkDessertMessage(message)) {
        if ((result = dessert_meshsend_fast(message, interface)) == DESSERT_OK) {
            // DEBUG: 
            std::cerr << "[TestbedPacketDispatcher::dispatch] sending message was successful" << std::endl;
        } else if(result == EINVAL) {
             // DEBUG: 
             std::cerr << "[TestbedPacketDispatcher::dispatch] message was broken" << std::endl;
        } else if(result == EIO) {
             // DEBUG: 
             std::cerr << "[TestbedPacketDispatcher::dispatch] message was not sent succesfully" << std::endl;
        } else {
             // DEBUG: 
             std::cerr << "[TestbedPacketDispatcher::dispatch] unknown error code" << std::endl;
        }
    } else {
        // DEBUG: 
        std::cerr << "[TestbedPacketDispatcher::dispatch] The message check failed and hence, the message was not sent" << std::endl;
    }

    return DESSERT_MSG_DROP;
}



dessert_cb_result toMesh(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags, dessert_sysif_t *interface, dessert_frameid_t id) {
    // DEBUG: std::cerr << "[toMesh] received frame " << ((unsigned long)id) << " on interface " << interface->if_name << std::endl;
    // DEBUG: dumpDessertMessage(message, length, flags);

    TestbedPacketFactory* packetFactory = dynamic_cast<TestbedPacketFactory*>(client->getPacketFactory());
    // DEBUG: std::cerr << "[toMesh] got a packet" << std::endl;
    TestbedPacket* packet = packetFactory->makeNewPacket(message);
    // DEBUG: std::cerr << "[toMesh] packet dump:" << std::endl;
    // DEBUG: std::cerr << *packet << std::endl;
    // DEBUG: std::cerr << "[toMesh] pass packet to client" << std::endl;

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

std::string toString(dessert_msg_t* message){
    std::ostringstream result;

    TestbedAddress nextHop(message->l2h.ether_dhost);
    TestbedAddress previousHop(message->l2h.ether_shost);

    result << "  " << "next hop (l2_dhost):       " << nextHop.toString() << std::endl;  
    result << "  " << "prev hop (l2_shost):       " << previousHop.toString() << std::endl;  
    result << "  " << "ethertype (l2_type):       " << std::hex << ntohs(message->l2h.ether_type) << std::endl;  
    result << std::endl;

    result << "  " << "proto:                     " << message->proto << std::endl;  
    result << "  " << "version:                   " << message->ver << std::endl;  
    result << std::endl;

    result << "  " << "ttl:                       " << message->ttl << std::endl;  
    result << "  " << "ara flags (u8):            " << message->u8 << std::endl;  
    result << "  " << "ara sequence number (u16): " << message->u16 << std::endl;  
    result << "  " << "header length (hlen):      " << ntohs(message->hlen) << std::endl;  
    result << "  " << "payload length (plen):     " << ntohs(message->plen) << std::endl;  
    result << std::endl;

    struct ether_header* l25h;

    if ((l25h = dessert_msg_getl25ether(message)) != nullptr) {    
        TestbedAddress source(l25h->ether_shost);
        TestbedAddress destination(l25h->ether_dhost);

        result << "  " << "l25 proto: ethernet    " << std::endl;
        result << "  " << "l25_dhost:             " << destination.toString() << std::endl;  
        result << "  " << "l25_shost:             " << source.toString() << std::endl;  
        result << "  " << "l25_type:              " << ntohs(l25h->ether_type) << std::endl;  
    }

    return result.str();
}

TESTBED_NAMESPACE_END
