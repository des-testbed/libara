 // --- LIBRARIES --- //
 #include "des-lsr.h"
 #include "des-lsr_items.h"

 // --- DAEMON INITIALIZATION --- //
 int main (int argc, char** argv) {
    // initialize daemon with correct parameters
    FILE *cfg = NULL;
    if ((argc == 2) && (strcmp(argv[1], "-nondaemonize") == 0)) {
        dessert_info("starting LSR in non daemonize mode");
        dessert_init("LSR", 0x02, DESSERT_OPT_NODAEMONIZE);
        char cfg_file_name[] = "/etc/des-lsr.conf";
        cfg = fopen(cfg_file_name, "r");
        if (cfg == NULL) {
            printf("Config file '%s' not found. Exit... \n", cfg_file_name);
            return EXIT_FAILURE;
        }
    } else {
        dessert_info("starting LSR in daemonize mode");
        cfg = dessert_cli_get_cfg(argc, argv);
        dessert_init("LSR", 0x03, DESSERT_OPT_NODAEMONIZE);
    }

    // periodic function initialization
    init_logic();

    // logging initialization
    dessert_logcfg(DESSERT_LOG_STDERR);

    // cli initialization
    cli_register_command(dessert_cli, dessert_cli_cfg_iface, "sys", dessert_cli_cmd_addsysif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "initialize sys interface");
    cli_register_command(dessert_cli, dessert_cli_cfg_iface, "mesh", dessert_cli_cmd_addmeshif, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "initialize mesh interface");

    struct cli_command* cli_cfg_set = cli_register_command(dessert_cli, NULL, "set", NULL, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set variable");
    cli_register_command(dessert_cli, cli_cfg_set, "hello_interval", cli_set_hello_interval, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set HELLO packet interval");
    cli_register_command(dessert_cli, dessert_cli_show, "hello_interval", cli_show_hello_interval, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "show HELLO packet size");
    cli_register_command(dessert_cli, cli_cfg_set, "tc_interval", cli_set_tc_interval, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set TC packet interval");
    cli_register_command(dessert_cli, dessert_cli_show, "tc_interval", cli_show_tc_interval, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "show TC packet size");
    cli_register_command(dessert_cli, cli_cfg_set, "refresh_list", cli_set_refresh_list, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set refresh NH interval");
    cli_register_command(dessert_cli, dessert_cli_show, "refresh_list", cli_show_refresh_list, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "show refresh NH interval");
    cli_register_command(dessert_cli, cli_cfg_set, "refresh_rt", cli_set_refresh_rt, PRIVILEGE_PRIVILEGED, MODE_CONFIG, "set refresh RT interval");
    cli_register_command(dessert_cli, dessert_cli_show, "refresh_rt", cli_show_refresh_rt, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "show refresh RT interval");
    cli_register_command(dessert_cli, dessert_cli_show, "rt", cli_show_rt, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "show RT");

    // callback registration
    //dessert_sysrxcb_add(sys_to_mesh, 10);
    dessert_meshrxcb_add(dessert_msg_ifaceflags_cb, 20);
    /*dessert_meshrxcb_add(drop_errors, 30);
    dessert_meshrxcb_add(process_hello, 40);
    dessert_meshrxcb_add(process_tc, 50);
    dessert_meshrxcb_add(forward_packet, 60);
    dessert_meshrxcb_add(mesh_to_sys, 70);
    */

    // running cli & daemon
    cli_file(dessert_cli, cfg, PRIVILEGE_PRIVILEGED, MODE_CONFIG);
    dessert_cli_run();
    dessert_run();
    return (0);
 }
