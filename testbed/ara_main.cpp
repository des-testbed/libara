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



int main(int argc, char** argv) {
     FILE* cfg = dessert_cli_get_cfg(argc, argv);
     if(dessert_init("ARAT", 0x00, DESSERT_OPT_NODAEMONIZE)!= DESSERT_OK) {
         cout << "dessert_init failed"<<endl;
     }
     else {
         cout << "dessert init succeded" << endl;
     }

     dessert_logcfg(DESSERT_LOG_STDERR | DESSERT_LOG_GZ); // enable compression, maybe this helps
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, "sys", dessert_cli_cmd_addsysif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "initialize tap interface");
     cli_register_command(dessert_cli, dessert_cli_cfg_iface, "mesh", dessert_cli_cmd_addmeshif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "initialize mesh interface");

     _dessert_cb_results (*ToMesh)(dessert_msg_t*, uint32_t, dessert_msg_proc_t*, dessert_sysif_t*, dessert_frameid_t) = &PacketToMeshDispatcher;
     dessert_sysrxcb_add(ToMesh, 50);
     dessert_msg_t *TestPacket = NULL;
     dessert_msg_new(&TestPacket);


     dessert_debug("applying configuration");
     cli_file(dessert_cli, cfg, PRIVILEGE_PRIVILEGED, MODE_CONFIG);
     dessert_debug("configuration applied");

     dessert_cli_run();
     /**
      * starts up the dessert main loop and waits until dessert_exit()
      * is called or the daemon is killed
      */
     dessert_run();

     if (TestPacket != NULL) {
        dessert_syssend_msg(TestPacket);
     }

     dessert_exit();

     return (0);
 }
