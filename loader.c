#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "loader.h"

static int stations_callback(void *void_node_list, int argc, char **argv, char **column_name)
{
    struct node *node = NULL;
    struct node_list *node_list = (struct node_list *)void_node_list;

    node = node_new();
    node->id = atoi(argv[0]);
    node_set_name(node, argv[1]);

    node_list_add(node_list, node);

    return 0;
}

static int schedules_callback(void *void_node_list, int argc, char **argv, char **column_name)
{
    struct node_list *node_list = (struct node_list *)void_node_list;
    struct node *start_node = NULL;
    struct node *end_node = NULL;

    start_node = node_list_find_by_id(node_list, atoi(argv[0]));
    if(NULL == start_node) {
        printf("Could not find start node with id %d\n", atoi(argv[0]));
        return 1;
    }

    end_node = node_list_find_by_id(node_list, atoi(argv[1]));
    if(NULL == end_node) {
        printf("Could not find end node with id %d\n", atoi(argv[1]));
        return 1;
    }

    node_add_neighbour(start_node, end_node);

    return 0;
}

int read_from_database(char *database_file_name, struct node_list *node_list)
{
    sqlite3 *db;
    char *error_msg = NULL;
    int error;

    /* Connect to the database */
    error = sqlite3_open(database_file_name, &db);
    if(error != SQLITE_OK)
    {
        printf("Could not open database file %s\n", database_file_name);
        goto out;
    }

    /* Get stations */
    error = sqlite3_exec(db, "select id, name from stations;",
        stations_callback, node_list, &error_msg);
    if(error != SQLITE_OK)
    {
        printf("Could not get stations data from database\n");
        goto out;
    }

    /* Get schedules */
    error = sqlite3_exec(db, "select start_station_id, end_station_id, line_id from schedules;", schedules_callback, node_list, &error_msg);
    if(error != SQLITE_OK)
    {
        printf("Could not get schedules data from database\n");
        goto out;
    }

    sqlite3_close(db);

    return 0;

out:
    if(NULL != error_msg) sqlite3_free(error_msg);
    sqlite3_close(db);
    return 1;
}
