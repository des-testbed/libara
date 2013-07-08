/*
 * $FU-Copyright$
 */

#include <iostream>
#include "CLibs.h"
#include "PacketDispatcher.h"
#include "TestbedTimer.h"

typedef u_char ara_address_t[ETHER_ADDR_LEN];

using namespace std;

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

     _dessert_cb_results (*fromNetwork)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_meshif_t*, dessert_frameid_t) = &ARA::testbed::messageFromNetworkDispatcher;
     dessert_cb_result (*toNetwork)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_sysif_t*, dessert_frameid_t) = &ARA::testbed::messageToNetworkDispatcher;

     dessert_sysrxcb_add(toNetwork, 50);
     dessert_meshrxcb_add(dessert_msg_ifaceflags_cb, 15);
     dessert_meshrxcb_add(fromNetwork, 50);

     dessert_debug("applying configuration");
     cli_file(dessert_cli, cfg, PRIVILEGE_PRIVILEGED, MODE_CONFIG);
     dessert_debug("configuration applied");

     dessert_cli_run();
     dessert_run();

     return (0);
 }
