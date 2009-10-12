#include <stdio.h>
#include <stdlib.h>

#include "loader.h"
#include "path_finder.h"

static int parse_cmdline(int argc, char **argv, char **start_station_str, char **end_station_str)
{
    if(argc != 3) {
        return 1;
    }

    *start_station_str = argv[1];
    *end_station_str = argv[2];

    return 0;
}

int main(int argc, char **argv)
{
    struct node_list *node_list = NULL;
    struct node_list *path = NULL;
    struct node_list_item *station = NULL;
    struct node *start_station = NULL;
    struct node *end_station = NULL;
    char *start_station_str = NULL;
    char *end_station_str = NULL;
    int error;

    error = parse_cmdline(argc, argv, &start_station_str, &end_station_str);
    if(error)
    {
        printf("usage: %s [start_station] [end_station]\n", *argv);
        return error;
    }

    node_list = node_list_new();
    error = read_from_database("ratp.db", node_list);
    if(error)
    {
        printf("Could not read from database\n");
        goto out;
    }

    path = node_list_new();

    start_station = node_list_find_by_name(node_list, start_station_str);
    if(NULL == start_station)
    {
        printf("Could not find start station \"%s\"\n", start_station_str);
        goto out;
    }

    end_station = node_list_find_by_name(node_list, end_station_str);
    if(NULL == end_station)
    {
        printf("Could not find end station \"%s\"\n", end_station_str);
        goto out;
    }

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
