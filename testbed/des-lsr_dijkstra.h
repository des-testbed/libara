 void shortest_path (uint8_t *addr) {
    if (!all_nodes_head) {
        return;
    }

    all_nodes_t *node = find_addr(addr);
    all_nodes_t *current = all_nodes_head;
    node_neighbors_t *current_neighbors;
    all_nodes_t *ptr = all_nodes_head;

    // Part I
    // set all nodes as unvisited, distance to infinity and prev hop to broadcast
    while (current) {
        current->visited = 0;
        current->distance = INFINITY;
        memcpy(current->prev_hop, ether_broadcast, ETH_ALEN * sizeof(u_int8_t));
        memcpy(current->next_hop, ether_broadcast, ETH_ALEN * sizeof(u_int8_t));
        current = current->hh.next;
    }

    // Part II
    // set distance of start node to 0 and prev to itself
    node->distance = 0;
    memcpy(node->prev_hop, node->addr, ETH_ALEN * sizeof(u_int8_t));
    memcpy(node->next_hop, node->addr, ETH_ALEN * sizeof(u_int8_t));

    // Part III
    // while there are some unvisited nodes
    while (!check_visited()) {
        // set unvisited node with lowest distance as current and visited
        current = get_low_unvisit_dist();
        current->visited = 1;

        // for all unvisited neighbors of current
        current_neighbors = current->neighbors;
        while (current_neighbors) {
            ptr = find_addr(current_neighbors->addr);

            // if old distance is larger then new; overwrite and set current to prev hop
            if (ptr) {
                if (current->distance + current_neighbors->weight < ptr->distance) {
                    ptr->distance = current->distance + current_neighbors->weight;
                    memcpy(ptr->prev_hop, current->addr, ETH_ALEN * sizeof(u_int8_t));
                }
            }
            current_neighbors = current_neighbors->hh.next;
        }
    }

    // PART IV
    // find next_hops for all nodes (go routing tree back to source)
    node = all_nodes_head;
    while (node) {
        if (memcmp(node->addr, addr, ETH_ALEN * sizeof(int))) {
            ptr = find_addr(node->prev_hop);
            if (memcmp(node->prev_hop, ether_broadcast, ETH_ALEN)) {
                while (memcmp(ptr->prev_hop, addr, ETH_ALEN * sizeof(u_int8_t))) {
                    ptr = find_addr(ptr->prev_hop);
                }
                memcpy(node->next_hop, ptr->addr, ETH_ALEN * sizeof(u_int8_t));
            }
        }
        node = node->hh.next;
    }
 }
