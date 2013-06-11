#include "testbed/CLibs.h"
//#include "AbstractARAClient.h"

#include "TestbedProxy.h"


typedef struct __attribute__((__packed__)) _my_extension {
   uint8_t value1;
   uint16_t value2;
   char char1;
} my_extension;

enum extensions {
   EXAMPLE_EXT_1 = DESSERT_EXT_USER,
   EXAMPLE_EXT_2
};

int toMesh(dessert_msg_t* msg, size_t len, dessert_msg_proc_t *proc, dessert_sysif_t *tunif, dessert_frameid_t id) {
   my_extension *mext = NULL;
   dessert_ext_t *ext = NULL;
   dessert_msg_addext(msg, &ext, EXAMPLE_EXT_1, sizeof(my_extension));
   mext = (my_extension*) &(ext->data);
   mext->value1 = 123;
   mext->value2 = 45678;
   mext->char1 = 'X';
   dessert_meshsend(msg, NULL);
   return DESSERT_MSG_DROP;
}

int toSys(dessert_msg_t* msg, size_t len, dessert_msg_proc_t *proc, const dessert_meshif_t *iface, dessert_frameid_t id) {
   struct ether_header *eth;
   size_t eth_len;

   if (proc->lflags & DESSERT_RX_FLAG_L25_DST ||
         proc->lflags & DESSERT_RX_FLAG_L25_BROADCAST ||
         proc->lflags & DESSERT_RX_FLAG_L25_MULTICAST ) {
             eth_len = dessert_msg_ethdecap(msg, &eth);
             dessert_syssend(eth, eth_len);
             free(eth);
   }

   return DESSERT_MSG_KEEP;
}

int main(int argc, char** argv) {
     /// read the configuration file 
     FILE* cfg = dessert_cli_get_cfg(argc, argv);
     /// initializes dessert framework and sets up logging
     dessert_init("ARAX", 0x00, DESSERT_OPT_DAEMONIZE);
     /// enable logging to STDERR 
     dessert_logcfg(DESSERT_LOG_STDERR | DESSERT_LOG_GZ); 
     /** initialize the command line interface */
     //ara_init_cli();

     /** initializes the routing table processing */
     //ara_rt_init();
     /** set up loop protection table */
     //ara_loopprotect_init();
     /** set up path classification table */
     //ara_classification_init();
     /** initializes the acknowledgement processing*/
     //ara_ack_init();

     /** registers the ara specific callbacks for libdessert*/
     //ara_register_cb();
     /** registers the tasks which will periodically executed */
     //ara_register_tasks();

     dessert_sysrxcb_add(toMesh, 50);
     dessert_meshrxcb_add(dessert_msg_ifaceflags_cb, 15);
     dessert_meshrxcb_add(toSys, 50);

     /** write debug message */
     dessert_debug("applying config");
     cli_file(dessert_cli, cfg, PRIVILEGE_PRIVILEGED, MODE_CONFIG);
     /** write debug message */
     dessert_debug("config applied");

     /** start up the command line interface. */
     dessert_cli_run();
     /**
      * starts up the dessert main loop and waits until dessert_exit()
      * is called or the daemon is killed
      */
     dessert_run();

     return (0);
 }
