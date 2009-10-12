#ifndef _NODE_H
#define _NODE_H

struct node;

struct node_list_item {
    struct node *item;
    struct node_list_item *next;
    struct node_list_item *previous;
};

struct node_list {
    struct node_list_item *head;
    struct node_list_item *tail;
};

struct node {
    int id;
    char *name;
    int name_length;
    int distance;
    int visited;
    struct node *previous;
    struct node_list *neighbours;
};

struct node *node_new();
void node_free(struct node *node);
void node_set_name(struct node *node, char *name);
void node_add_neighbour(struct node *node, struct node *neighbour);

struct node_list *node_list_new();
void node_list_free(struct node_list *node_list, int free_nodes);
void node_list_append(struct node_list *node_list, struct node_list *new_node_list);
void node_list_map(struct node_list *node_list, void (*actor)(struct node_list_item *, void *), void *data);
void node_list_add(struct node_list *node_list, struct node *node);
void node_list_remove(struct node_list *node_list, struct node_list_item *node_list_item);
struct node *node_list_find(struct node_list *node_list, int (*criteria)(struct node *, void *), void *data);
struct node *node_list_find_by_id(struct node_list *node_list, int id);
struct node *node_list_find_by_name(struct node_list *node_list, char *name);

#endif
