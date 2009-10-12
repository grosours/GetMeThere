#ifndef _NODE_H
#define _NODE_H

struct node;

struct node_list {
    struct node *head;
    struct node *tail;
};

struct node {
    int id;
    char *name;
    int name_length;
    int distance;
    struct node_list *neighbours;
    struct node *next;
};

struct node *node_new();
void node_free(struct node *node);
void node_set_name(struct node *node, char *name);
void node_add_neighbour(struct node *node, struct node *neighbour);

struct node_list *node_list_new();
void node_list_free(struct node_list *node_list);
void node_list_add(struct node_list *node_list, struct node *node);
struct node *node_list_find_by_id(struct node_list *node_list, int id);
struct node *node_list_find_by_name(struct node_list *node_list, char *name);

#endif