#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "path_finder.h"

void lowest_distance_actor(struct node_list_item *node_list_item, void *void_node_list_item_ptr)
{
    struct node_list_item **current_node_list_item_ptr = (struct node_list_item **)void_node_list_item_ptr;

    if(node_list_item->item->distance < (*current_node_list_item_ptr)->item->distance) {
        *current_node_list_item_ptr = node_list_item;
    }
}

void set_initial_distance_actor(struct node_list_item *node_list_item, void *whatever)
{
    node_list_item->item->distance = INT_MAX;
    node_list_item->item->visited = 0;
    node_list_item->item->previous = NULL;
}

void relax_actor(struct node_list_item *node_list_item, void *void_node_ptr)
{
    struct node *current_node_ptr = (struct node *)void_node_ptr;

    if(0 == node_list_item->item->visited) {
        node_list_item->item->distance = current_node_ptr->distance + 2;
        node_list_item->item->previous = current_node_ptr;
    }
}

int find_path(struct node_list *node_list, struct node_list *path, struct node *start_node, struct node *end_node)
{
    struct node_list *node_pool = NULL;
    struct node_list_item *current_node_list_item = NULL;

    if(NULL == node_list) return 1;
    if(NULL == path) return 1;
    if(NULL == start_node) return 1;
    if(NULL == end_node) return 1;
    if(NULL == node_list->head) return 1;

    node_pool = node_list_new();
    node_list_add_list(node_list, node_pool);

    /* Set the nodes distance */
    node_list_map(node_pool, set_initial_distance_actor, NULL);
    start_node->distance = 0;

    current_node_list_item = node_pool->head;
    while(NULL != current_node_list_item) {
        /* Find the node with the lowest distance */
        node_list_map(node_pool, lowest_distance_actor, &current_node_list_item);
        if(current_node_list_item->item->distance == INT_MAX) {
            printf("Could not find path\n");
            goto end;
        }

        if(current_node_list_item->item == end_node) {
            struct node *my_node = end_node;
            while(NULL != my_node) {
                node_list_add(path, my_node);
                my_node = my_node->previous;
            }

            break;
        }

        current_node_list_item->item->visited = 1;
        node_list_map(current_node_list_item->item->neighbours, relax_actor, current_node_list_item->item);

        node_list_remove(node_pool, current_node_list_item);
        current_node_list_item = node_pool->head;
    }

    node_list_free(node_pool, 0);

    return 0;

end:
    node_list_free(node_pool, 0);

    return 1;
}
