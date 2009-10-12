#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "node.h"

struct node *node_new()
{
    struct node *node;

    node = (struct node *)malloc(sizeof(struct node));
    assert(NULL != node);

    node->id = -1;
    node->name = NULL;
    node->name_length = 0;
    node->distance = INT_MAX;
    node->visited = 0;
    node->neighbours = node_list_new();

    return node;
}

void node_free(struct node *node)
{
    if(NULL == node) return;

    if(NULL != node->name) free(node->name);
    node_list_free(node->neighbours, 0);
    free(node);
}

void node_set_name(struct node *node, char *name)
{
    if(NULL == node) return;

    if(NULL != node->name) free(node->name);
    if(NULL == name) {
        node->name = NULL;
    } else {
        node->name_length = strlen(name);
        node->name = (char *)malloc(node->name_length * sizeof(char));
        assert(NULL != node->name);
        strncpy(node->name, name, node->name_length);
    }
}

void node_add_neighbour(struct node *node, struct node *neighbour)
{
    if(NULL == node) return;

    node_list_add(node->neighbours, neighbour);
}

/* Node node_list */

struct node_list *node_list_new()
{
    struct node_list *node_list;

    node_list = (struct node_list *)malloc(sizeof(struct node_list));
    assert(NULL != node_list);

    node_list->head = NULL;
    node_list->tail = NULL;

    return node_list;
}

void node_list_free(struct node_list *node_list, int free_nodes)
{
    struct node_list_item *current = NULL;
    struct node_list_item *next = NULL;

    if(NULL == node_list) return;

    current = node_list->head;
    while(current != NULL) {
        next = current->next;
        if(free_nodes) node_free(current->item);
        free(current);
        current = next;
    }

    free(node_list);
}

void node_list_reverse(struct node_list *node_list)
{
    struct node_list_item *current = NULL;
    struct node_list_item *next = NULL;

    if(NULL == node_list) return;
    current = node_list->head;

    node_list->head = node_list->tail;
    node_list->tail = current;

    while(current != NULL) {
        next = current->next;
        current->next = current->previous;
        current->previous = next;
        current = next;
    }
}

void node_list_add(struct node_list *node_list, struct node *node)
{
    struct node_list_item *node_list_item = NULL;

    if(NULL == node_list) return;
    if(NULL == node) return;

    node_list_item = (struct node_list_item *)malloc(sizeof(struct node_list_item));
    assert(NULL != node_list_item);
    node_list_item->item = node;

    if(NULL == node_list->tail) {
        node_list->head = node_list->tail = node_list_item;
        node_list_item->previous = NULL;
    } else {
        node_list_item->previous = node_list->tail;
        node_list->tail->next = node_list_item;
        node_list->tail = node_list_item;
    }

    node_list_item->next = NULL;
}

void node_list_remove(struct node_list *node_list, struct node_list_item *node_list_item)
{
    if(NULL == node_list) return;
    if(NULL == node_list->tail) return;
    if(NULL == node_list_item) return;

    if(node_list_item->previous) {
        node_list_item->previous->next = node_list_item->next;
    }

    if(node_list_item->next) {
        node_list_item->next->previous = node_list_item->previous;
    }

    if(node_list->head == node_list_item) {
        node_list->head = node_list_item->next;
    }

    if(node_list->tail == node_list_item) {
        node_list->tail = node_list_item->previous;
    }

    free(node_list_item);
}

void node_list_map(struct node_list *node_list, void (*actor)(struct node_list_item *, void *), void *data)
{
    struct node_list_item *current = NULL;
    struct node_list_item *next = NULL;

    if(NULL == node_list) return;
    current = node_list->head;

    while(current != NULL) {
        actor(current, data);
        next = current->next;
        current = next;
    }
}

static void node_list_add_list_actor(struct node_list_item *node_list_item, void *void_node_list)
{
    struct node_list *node_list = (struct node_list *)void_node_list;
    node_list_add(node_list, node_list_item->item);
}

void node_list_add_list(struct node_list *node_list, struct node_list *new_node_list)
{
    if(NULL == node_list) return;
    if(NULL == new_node_list) return;

    node_list_map(node_list, node_list_add_list_actor, new_node_list);
}

struct node *node_list_find(struct node_list *node_list, int (*criteria)(struct node *, void *), void *data)
{
    struct node_list_item *current = NULL;
    struct node_list_item *next = NULL;

    if(NULL == node_list) return NULL;
    current = node_list->head;

    while(current != NULL) {
        if(criteria(current->item, data)) return current->item;
        next = current->next;
        current = next;
    }

    return NULL;
}

static int id_criteria(struct node *node, void *void_id)
{
    return (node->id == (int)void_id);
}

struct node *node_list_find_by_id(struct node_list *node_list, int id)
{
    return node_list_find(node_list, id_criteria, (void *)id);
}

static int name_criteria(struct node *node, void *void_name)
{
    return (0 == strncmp(node->name, (char *)void_name, node->name_length));
}

struct node *node_list_find_by_name(struct node_list *node_list, char *name)
{
    return node_list_find(node_list, name_criteria, (void *)name);
}
