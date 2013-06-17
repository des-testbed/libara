#include <iostream>
#include "CLibs.h"
//#include "AbstractARAClient.h"
#include "TestbedPacketDispatcher.h"


using namespace std;

_dessert_cb_results PacketToMeshDispatcher (dessert_msg_t* ReceivedMessage, uint32_t Length, dessert_msg_proc_t* ProcessingFlags, dessert_sysif_t* SystemInterface, dessert_frameid_t id) {
    cout <<"Received packet from sys, broadcasting via mesh"<<endl;
    if(dessert_meshsend(ReceivedMessage,NULL)==DESSERT_OK)
        return DESSERT_MSG_DROP;
    else return DESSERT_MSG_NEEDNOSPARSE;
}

_dessert_cb_results PacketToSystemDispatcher (dessert_msg_t* ReceivedMessage, uint32_t Length, dessert_msg_proc_t *ProcessingFlags, const dessert_meshif_t *MeshInterface, dessert_frameid_t id);

int testbed_cli_cmd_testsendmesh(struct cli_def* cli, char* command, char* argv[], int argc){
    dessert_msg_t *TestPacket = NULL;
    dessert_msg_new(&TestPacket);
    if(TestPacket != NULL) {
        dessert_syssend_msg(TestPacket);
        return 0;
    }
    else {
        return 1;
    }

}


int main(int argc, char** argv) {
     FILE* cfg = dessert_cli_get_cfg(argc, argv);
     if(dessert_init("ARAT", 0x00, DESSERT_OPT_NODAEMONIZE)!= DESSERT_OK) {
         cout << "dessert_init failed"<<endl;
     }
     else {
         cout << "dessert init succeded" << endl;
     }

     dessert_logcfg(DESSERT_LOG_STDERR | DESSERT_LOG_GZ); // enable compression, maybe this helps
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("sys"), dessert_cli_cmd_addsysif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize tap interface"));
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, const_cast<char*>("mesh"), dessert_cli_cmd_addmeshif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, const_cast<char*>("initialize mesh interface"));
     cli_register_command(dessert_cli, dessert_cli_show, const_cast<char*>("testSendMesh"), testbed_cli_cmd_testsendmesh, PRIVILEGE_UNPRIVILEGED, MODE_ANY, const_cast<char*>("send a test packet to mesh interface"));

     _dessert_cb_results (*ToMesh)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_sysif_t*, dessert_frameid_t) = &PacketToMeshDispatcher;
     dessert_sysrxcb_add(ToMesh, 50);

     dessert_debug("applying configuration");
     cli_file(dessert_cli, cfg, PRIVILEGE_PRIVILEGED, MODE_CONFIG);
     dessert_debug("configuration applied");

     dessert_cli_run();
     dessert_run();

     return (0);
 }
