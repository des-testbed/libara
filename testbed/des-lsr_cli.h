 int cli_set_hello_interval(struct cli_def *cli, char *command, char *argv[], int argc) {
    if(argc != 1) {
        cli_print(cli, "usage %s [interval]\n", command);
        return CLI_ERROR;
    }

    hello_interval = (u_int16_t) strtoul(argv[0], NULL, 10);
    dessert_periodic_del(periodic_send_hello);
    struct timeval hello_interval_t;
    hello_interval_t.tv_sec = hello_interval / 1000;
    hello_interval_t.tv_usec = (hello_interval % 1000) * 1000;
    periodic_send_hello = dessert_periodic_add(send_hello, NULL, NULL, &hello_interval_t);
    dessert_notice("setting HELLO interval to %d ms\n", hello_interval);
     return CLI_OK;
 }

 int cli_show_hello_interval(struct cli_def *cli, char *command, char *argv[], int argc) {
    cli_print(cli, "HELLO interval = %d ms\n", hello_interval);
     return CLI_OK;
 }
