#include "path_finder.h"

int find_path(struct node_list *node_list, struct node_list *path)
{
    struct node_list *node_pool;

    node_pool = node_list_new();
    node_list_append(node_list, node_pool);

    

    node_list_free(node_pool, 0);

    return 0;
}
