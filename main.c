#include <stdio.h>
#include <stdlib.h>

#include "loader.h"
#include "path_finder.h"

int main(int argc, char **argv)
{
    struct node_list *node_list = NULL;
    struct node_list *path = NULL;
    struct node_list_item *station = NULL;
    struct node *start_station = NULL;
    struct node *end_station = NULL;
    int error;

    node_list = node_list_new();
    error = read_from_database("ratp.db", node_list);
    if(error)
    {
        printf("Could not read from database\n");
        goto out;
    }

    path = node_list_new();
    start_station = node_list_find_by_name(node_list, "Balard");
    end_station = node_list_find_by_name(node_list, "République");
    error = find_path(node_list, path, start_station, end_station);
    if(error)
    {
        printf("Could not find path\n");
        goto out;
    }

    printf("Found the path:\n");
    station = path->tail;
    while(NULL != station) {
        printf(" * %s\n", station->item->name);
        station = station->previous;
    }

    node_list_free(node_list, 1);

    return 0;

out:
    node_list_free(node_list, 1);
    node_list_free(path, 1);
    return 1;
}
