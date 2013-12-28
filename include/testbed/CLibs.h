/*
 * $FU-Copyright$
 */

#ifndef CLIBS_H
#define CLIBS_H

extern "C" {
 #include <dessert.h>
 #ifdef min
 #undef min
 #endif
 #ifdef max
 #undef max
 #endif
 #include <unistd.h>
 #include <pthread.h>
 #include <netinet/in.h>
 #include <uthash.h>
 #include <libcli.h>
}

#endif /* CLIBS_H */
