#include <stdio.h>
#include <stdlib.h>

#include "loader.h"
#include "path_finder.h"

int main(int argc, char **argv)
{
    struct node_list *node_list = NULL;
    struct node_list *path = NULL;
    int error;

    node_list = node_list_new();
    error = read_from_database("ratp.db", node_list);
    if(error)
    {
        printf("Could not read from database\n");
        goto out;
    }

    path = node_list_new();
    error = find_path(node_list, path);

    node_list_free(node_list, 1);

    return 0;

out:
    node_list_free(node_list, 1);
    node_list_free(path, 1);
    return 1;
}
