extern "C" {
#include <dessert.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <uthash.h>
}
//#include <libcli.h>

int main(int argc, char** argv) {
     /** read the configuration file */
     FILE* cfg = dessert_cli_get_cfg(argc, argv);
     /** initializes dessert framework and sets up logging */
     dessert_init("ARAX", 0x00, DESSERT_OPT_DAEMONIZE);
     /** enable logging to STDERR */
     dessert_logcfg(DESSERT_LOG_STDERR | DESSERT_LOG_GZ); // enable compression, maybe this helps
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
