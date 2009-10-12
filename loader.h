#ifndef _LOADER_H
#define _LOADER_H

#include "node.h"

static int stations_callback(void *void_node_list, int argc, char **argv, char **column_name);
static int schedules_callback(void *void_node_list, int argc, char **argv, char **column_name);

int read_from_database(char *database_file_name, struct node_list **node_list);
int main(int argc, char **argv);

#endif
