 #include "des-lsr.h"
 #include "des-lsr_items.h"

 int sys_to_mesh(dessert_msg_t *msg, size_t len, dessert_msg_proc_t *proc, dessert_sysif_t *sysif, dessert_frameid_t id) {
         struct ether_header* l25h = dessert_msg_getl25ether(msg);               // ptr to l2.5 header
         all_nodes_t* node;
         HASH_FIND(hh, all_nodes_head, l25h->ether_dhost, ETH_ALEN, node);       // finding destination of msg in hashmap

         // if destination and next hop for destination is known
         if (node && memcmp(node->next_hop, ether_broadcast, ETH_ALEN) != 0) {
                 memcpy(msg->l2h.ether_dhost, node->next_hop, ETH_ALEN);
                 dessert_meshsend_fast(msg, NULL);
         }

     return DESSERT_MSG_DROP;
 }

 int mesh_to_sys(dessert_msg_t* msg, size_t len, dessert_msg_proc_t *proc, const dessert_meshif_t *iface, dessert_frameid_t id) {
         if((proc->lflags & DESSERT_RX_FLAG_L25_DST && !(proc->lflags & DESSERT_RX_FLAG_L25_OVERHEARD))
                         || proc->lflags & DESSERT_RX_FLAG_L25_BROADCAST
                         || proc->lflags & DESSERT_RX_FLAG_L25_MULTICAST ) {
                 dessert_syssend_msg(msg);
         }

         return DESSERT_MSG_DROP;
 }
