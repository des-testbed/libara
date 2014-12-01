/*
 * $FU-Copyright$
 */

#include "TestbedPacket.h"
#include "TestbedPacketDispatcher.h"
#include "TestbedAddress.h"

#include <netinet/in.h>

//extern std::shared_ptr<ARA::testbed::TestbedARAClient> client;
extern ARA::testbed::TestbedARAClient* client;

TESTBED_NAMESPACE_BEGIN

/**
 * RECEIVE
 */
dessert_cb_result toSys(dessert_msg_t* message, uint32_t length, dessert_msg_proc_t *flags, dessert_meshif_t* interface, dessert_frameid_t id) {
    TestbedPacketFactory* packetFactory = dynamic_cast<TestbedPacketFactory*>(client->getPacketFactory());

    // let's check if the received message is consistent
    if (packetFactory->checkDessertMessage(message)) {
        TestbedPacket* packet = packetFactory->makeNewPacket(message);

        // DEBUG: std::cerr << "[toSys] packet dump " << std::endl;
        // DEBUG: dumpDessertMessage(message, length, flags);
        // DEBUG: std::cerr << " and again! " << std::endl;
        // DEBUG: std::cerr << toString(message, true);
        
        TestbedNetworkInterface* networkInterface = client->getTestbedNetworkInterface(std::make_shared<TestbedAddress>(interface->hwaddr));
        networkInterface->receive(packet);
    }

    return DESSERT_MSG_KEEP;
}

/**
* SEND
 */
int dispatch(const Packet* packet, std::shared_ptr<TestbedAddress> interfaceAddress, std::shared_ptr<Address> recipient) {
    assert(packet != nullptr);
    /// determine the mesh interface
    dessert_meshif_t* interface = dessert_meshif_get_hwaddr(interfaceAddress->getDessertValue());

    const TestbedPacket* testbedPacket = dynamic_cast<const TestbedPacket*>(packet);
    // create a dessert message out of the packet
    dessert_msg_t* message = testbedPacket->toDessertMessage();

    /// send the packet
    int result = 42;

    TestbedAddressPtr recipientAddress = std::dynamic_pointer_cast<TestbedAddress>(recipient);

    /// FIXME: 
    ara_address_t tmp; 
    /// FIXME: backup the address
    std::memcpy(tmp, message->l2h.ether_dhost, sizeof(ara_address_t));
    /// set the next hop as the destination host 
    std::memcpy(message->l2h.ether_dhost, recipientAddress->getDessertValue(), sizeof(ara_address_t));

    // DEBUG: 
    std::cerr << "[TestbedPacketDispatcher::dispatch] next hop is " << recipientAddress->toString() << std::endl;
    // DEBUG: std::cerr << "[toMesh] packet dump " <<  std::endl;
    // DEBUG: std::cerr << toString(message, true);

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
        std::cerr << "[TestbedPacketDispatcher::dispatch] send message with sequence nr. " << ntohs(message->u16) << 
          " and type " << PacketType::getAsString(testbedPacket->getType()) << std::endl;
        if ((result = dessert_meshsend_fast(message, interface)) == DESSERT_OK) {
            // DEBUG: std::cerr << "[TestbedPacketDispatcher::dispatch] sending message was successful" << std::endl;
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
    // TODO: check if this might become a problem if the sequence number is
    // actually going to be 48879/0xBEEF
    //if (ntohs(message->u16) == 48879) {
        /**
         * we set our own sequence number since it is the default sequence
         * number of libdessert (48879 == 0xBEEF)
         */
        message->u16 = htons(client->getNextSequenceNumber());
    //}

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

std::string toString(dessert_msg_t* message, bool isNetworkByteOrder){
    std::ostringstream result;

    TestbedAddress nextHop(message->l2h.ether_dhost);
    TestbedAddress previousHop(message->l2h.ether_shost);

    result << "  " << "next hop (l2_dhost):       " << nextHop.toString() << std::endl;  
    result << "  " << "prev hop (l2_shost):       " << previousHop.toString() << std::endl;  

    result << "  " << "ethertype (l2_type):       ";
    if (isNetworkByteOrder) {
        result << std::hex << ntohs(message->l2h.ether_type) << std::endl;  
    } else {
        result << std::hex << message->l2h.ether_type << std::endl; 
    }

    result << "  " << "proto:                     " << message->proto << std::endl;  
    result << "  " << "version:                   " << std::dec << (int)message->ver << std::endl;  

    result << "  " << "flags:                     ";
    if (message->flags & DESSERT_RX_FLAG_SPARSE) {
        result << " SPARSE"; 
    } 

    result << std::endl << std::endl;

    result << "  " << "ttl:                       " << std::dec << (int)message->ttl << std::endl;  
    result << "  " << "ara flags (u8):            " << std::dec << (int)message->u8 << " (" << PacketType::getAsString(message->u8) << ")" << std::endl;  

    result << "  " << "ara sequence number (u16): ";
    if (isNetworkByteOrder) {
        result << ntohs(message->u16) << std::endl;  
    } else {
        result << message->u16 << std::endl;  
    }

    result << "  " << "header length (hlen):      ";  
    if (isNetworkByteOrder) {
        result << ntohs(message->hlen) << std::endl;  
    } else {
        result << message->hlen << std::endl;  
    }

    result << "  " << "payload length (plen):     ";  
    if (isNetworkByteOrder) {
        result << ntohs(message->plen) << std::endl;  
    } else {
        result << message->plen << std::endl;  
    }

    result << std::endl;

    struct ether_header* l25h;

    if ((l25h = dessert_msg_getl25ether(message)) != nullptr) {    
        TestbedAddress source(l25h->ether_shost);
        TestbedAddress destination(l25h->ether_dhost);

        result << "  " << "l25 proto: ethernet        " << std::endl;
        result << "  " << "l25_dhost:                 " << destination.toString() << std::endl;  
        result << "  " << "l25_shost:                 " << source.toString() << std::endl;  
        result << "  " << "l25_type:                  " << ntohs(l25h->ether_type) << std::endl;  
        result << std::endl;
    }

    /// check if there is a trace
    std::string trace = toStringTrace(message, DESSERT_EXT_TRACE_REQ);
    if (trace.length() > 0) {
        result << trace << std::endl;
    }

    trace = toStringTrace(message, DESSERT_EXT_TRACE_RPL);
    if (trace.length() > 0) {
        result << trace << std::endl;
    }

    int extensionIdentifier = 0;
    /// let's check for other extensions
    dessert_ext_t* extension = (dessert_ext_t*)((uint8_t*) message + sizeof(struct dessert_msg));

    while((uint8_t*) extension < ((uint8_t*) message + (uint32_t) ntohs(message->hlen))) {
        result << "  " << "extension:                 " << extensionIdentifier << std::endl;

        /// check if the current extension fits into the header
        if ((((uint8_t*) extension + (uint32_t) extension->len) > ((uint8_t*) message + (uint32_t) ntohs(message->hlen)))
           || (extension->len < 2)) {
            result << "  " << "broken extension - giving up!"  << std::endl;
            break;
        }

        result << "  " << "type:                      " << "0x" << std::hex << (int)extension->type << std::endl;
        result << "  " << "len:                       " << std::dec << (int)extension->len << std::endl;

        if (extension->type != DESSERT_EXT_ETH && extension->type != DESSERT_EXT_TRACE_REQ) {
            result << "  " << "data:                      ";
            unsigned int i;

            for(i = 0; i < dessert_ext_getdatalen(extension); i++) {
                result << "0x" << std::hex << (int)extension->data[i] << " ";

                if(i % 12 == 1 && i != 1) {
                    result << "\t\t";
                }
            }
        }

        result << std::endl;

        extension = (dessert_ext_t*)((uint8_t*) extension + (uint32_t) extension->len);
        extensionIdentifier++;
    }

    return result.str();
}

std::string toStringTrace(const dessert_msg_t* message, uint8_t type) {
    std::ostringstream result;

    if (type == DESSERT_EXT_TRACE_REQ || type == DESSERT_EXT_TRACE_RPL) {
        int x = 0;
        dessert_ext_t* extension;

        if ((x = dessert_msg_getext(message, &extension, type, 0)) >= 1) {
            TestbedAddress origin(extension->data);
            result << "  " << "packet trace from:          " << origin.toString() << std::endl;

            if (dessert_ext_getdatalen(extension) == DESSERT_MSG_TRACE_IFACE) {
                TestbedAddress receiver(extension->data);
                result << "  " << "received on:                " << receiver.toString() << std::endl;
                TestbedAddress source(extension->data + 12);
                result << "  " << "l2.5 src:                   " << source.toString() << std::endl;
            }

            for (int i = 1; i < x; i++) {
                 dessert_msg_getext(message, &extension, type, i);
                 TestbedAddress tmp(extension->data);
                 result << "  " << "                            " << tmp.toString() << std::endl;

                 if (dessert_ext_getdatalen(extension) == DESSERT_MSG_TRACE_IFACE) {
                     TestbedAddress sender(extension->data + 12);
                     result << "  " << "received from:              " << sender.toString() << std::endl;

                     TestbedAddress receiverMeshAddress(extension->data + 6);
                     result << "  " << "received meshif:            " << receiverMeshAddress.toString() << std::endl;
                 }
            }
        }
    }

    return result.str();
}

TESTBED_NAMESPACE_END
