#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "path_finder.h"

static int stations_callback(void *nodes, int argc, char **argv, char **column_name)
{
    struct node *node = NULL;

    printf("Got station %s with id %d\n", argv[1], atoi(argv[0]));

    return 0;
}

static int schedules_callback(void *nodes, int argc, char **argv, char **column_name)
{
    printf("Got schedule from %d to %d with line %d\n", atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));

    return 0;
}

int read_from_database(char *database_file_name, struct node_list **node_list)
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
        stations_callback, *node_list, &error_msg);
    if(error != SQLITE_OK)
    {
        printf("Could not get stations data from database\n");
        goto out;
    }

    /* Get schedules */
    error = sqlite3_exec(db, "select start_station_id, end_station_id, line_id from schedules;", schedules_callback, *node_list, &error_msg);
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

int main(int argc, char **argv)
{
    struct node_list *node_list = NULL;
    int error;

    node_list = node_list_new();
    error = read_from_database("ratp.db", &node_list);
    if(error)
    {
        printf("Could not read from database\n");
        node_list_free(node_list);
        return 1;
    }

    printf("Reading from database ok\n");

    node_list_free(node_list);

    return 0;
}
