/*
 * $FU-Copyright$
 */

#include <iostream>
#include "CLibs.h"
#include "PacketDispatcher.h"

typedef u_char ara_address_t[ETHER_ADDR_LEN];

using namespace std;

int testbed_cli_cmd_testsendmesh(struct cli_def* cli, char* command, char* argv[], int argc){
    /// DEBUG:
    cout << "prepare to send packet " << endl;

    /// set the address to 
    ///ara_address_t address = "00:21:9b:79:22:02";
    ara_address_t address;
    /// the packet which will encapsulate the data
    dessert_msg_t *testPacket = nullptr;
    /// define a new ethernet frame
    struct ether_header* ethernetFrame = nullptr;
    /// define a new extension record
    dessert_ext_t* extensionRecord = nullptr;
    /// generate a new packet 
    dessert_msg_new(&testPacket);
    /// set sequence number 
    testPacket->u16 = htons(42);
    /// set time to live (constant from des-ara code)
    testPacket->ttl = 64;
    /// set field reserved for app usage in dessert
    testPacket->u8 |= 0x01;

    /// add extension record to the message
    dessert_msg_addext(testPacket, &extensionRecord, DESSERT_EXT_ETH, ETHER_HDR_LEN);
    /// set the extension record as data to the ethernet frame
    ethernetFrame = (struct ether_header*) extensionRecord->data;
    /// set the source host
    memcpy(ethernetFrame->ether_shost, dessert_l25_defsrc, ETHER_ADDR_LEN);
    /// set the destination host
    memcpy(ethernetFrame->ether_dhost, ether_broadcast, ETHER_ADDR_LEN);

    /// FIXME
    memcpy(address, dessert_l25_defsrc, sizeof(ara_address_t));

    /// 
    dessert_msg_addext(testPacket, &extensionRecord, DESSERT_EXT_USER, ETHER_ADDR_LEN + 4);
    memcpy(extensionRecord->data, address, sizeof(ara_address_t));
    memcpy(extensionRecord->data + ETHER_ADDR_LEN, "FANT", 4);

    /// add dummy payload
    dessert_msg_dummy_payload(testPacket, 128);

    if(testPacket != nullptr) {
        cout << "send packet " << endl;
        dessert_meshsend(testPacket, NULL);
        cout << "packet sent" << endl;
        dessert_msg_destroy(testPacket);
        return 0;
    } 

    return 1;
}

int main(int argc, char** argv) {
     FILE* cfg = dessert_cli_get_cfg(argc, argv);

     if(dessert_init("ARAT", 0x00, DESSERT_OPT_NODAEMONIZE) != DESSERT_OK) {
         cout << "dessert_init failed" << endl;
     } else {
         cout << "dessert init succeded" << endl;
     }

     dessert_logcfg(DESSERT_LOG_STDERR | DESSERT_LOG_GZ); 

     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("sys"), dessert_cli_cmd_addsysif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize tap interface"));
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("mesh"), dessert_cli_cmd_addmeshif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize mesh interface"));
     cli_register_command(dessert_cli, dessert_cli_show, const_cast<char*>("testSendMesh"), testbed_cli_cmd_testsendmesh, PRIVILEGE_UNPRIVILEGED, MODE_ANY, const_cast<char*>("send a test packet to mesh interface"));

     /// ara_tun2ara
     _dessert_cb_results (*ToMesh)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_sysif_t*, dessert_frameid_t) = &PacketToMeshDispatcher;
     /// ara_ara2tun
     dessert_cb_result (*ToSys)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_meshif_t*, dessert_frameid_t) = &PacketToSystemDispatcher;

     dessert_sysrxcb_add(ToMesh, 50);
     dessert_meshrxcb_add(dessert_msg_ifaceflags_cb, 15);
     dessert_meshrxcb_add(ToSys, 50);

     dessert_debug("applying configuration");
     cli_file(dessert_cli, cfg, PRIVILEGE_PRIVILEGED, MODE_CONFIG);
     dessert_debug("configuration applied");

     dessert_cli_run();
     dessert_run();

     return (0);
 }
