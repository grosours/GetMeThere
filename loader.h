#ifndef _LOADER_H
#define _LOADER_H

#include "node.h"

int read_from_database(char *database_file_name, struct node_list *node_list);
int main(int argc, char **argv);

#endif
