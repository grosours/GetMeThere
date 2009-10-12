#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

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
    node->neighbours = node_list_new();
    node->next = NULL;

    return node;
}

void node_free(struct node *node)
{
    if(NULL == node) return;

    if(NULL != node->name) free(node->name);
    node_list_free(node->neighbours);
    free(node);
}

void node_set_name(struct node *node, char *name)
{
    if(NULL == node) return;

    if(NULL != node->name) free(node->name);
    if(NULL == name)
    {
        node->name = NULL;
    }
    else
    {
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

/* Node list */

struct node_list *node_list_new()
{
    struct node_list *node_list;

    node_list = (struct node_list *)malloc(sizeof(struct node_list));
    assert(NULL != node_list);

    node_list->head = NULL;
    node_list->tail = NULL;

    return node_list;
}

void node_list_free(struct node_list *node_list)
{
    struct node *current = NULL;
    struct node *next = NULL;

    if(NULL == node_list) return;

    current = node_list->head;
    while(current != NULL) {
        next = current->next;
        node_free(current);
        current = next;
    }

    free(node_list);
}

void node_list_add(struct node_list *node_list, struct node *node)
{
    if(NULL == node_list) return;
    if(NULL == node) return;

    if(NULL == node_list->tail) {
        node_list->head = node_list->tail = node;
        node->next = NULL;
    }
}

struct node *node_list_find_by_id(struct node_list *node_list, int id)
{
    struct node *current = NULL;
    struct node *next = NULL;

    if(NULL == node_list) return;
    current = node_list->head;

    while(current != NULL) {
        if(current->id == id) return current;
        next = current->next;
        current = next;
    }

    return NULL;
}

struct node *node_list_find_by_name(struct node_list *node_list, char *name)
{
    struct node *current = NULL;
    struct node *next = NULL;

    if(NULL == node_list) return;
    current = node_list->head;

    while(current != NULL) {
        if(0 == strncpy(current->name, name, current->name_length)) {
            return current;
        }
        next = current->next;
        current = next;
    }

    return NULL;
}
