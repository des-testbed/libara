extern "C"{
#include <dessert.h>
 #include <uthash.h>
}
 #include "des-lsr.h"

 #define INFINITY 200

 // DATABASE
 // a hashmap for all neighbours of a node
 typedef struct node_neighbors {
    u_int8_t addr[ETH_ALEN];
    u_int8_t entry_age;
    u_int8_t weight;
    UT_hash_handle hh;
 } __attribute__((__packed__)) node_neighbors_t;
 extern node_neighbors_t *dir_neighbors_head;

 // a hashmap for all nodes
 typedef struct all_nodes {
    u_int8_t addr[ETH_ALEN];
    u_int8_t next_hop[ETH_ALEN];
    u_int8_t prev_hop[ETH_ALEN];
    u_int8_t entry_age;
    u_int8_t seq_nr;
    u_int8_t distance;
    u_int8_t visited;
    node_neighbors_t* neighbors;
    UT_hash_handle hh;
 } all_nodes_t;
 extern all_nodes_t *all_nodes_head;

 // EXTENSIONS
 typedef struct  hello_ext {
 } __attribute__((__packed__)) hello_ext_t;
