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
    } else {
        node_list->tail->next = node_list_item;
        node_list->tail = node_list_item;
    }

    node_list_item->next = NULL;
}

struct node *node_list_find_by_id(struct node_list *node_list, int id)
{
    struct node_list_item *current = NULL;
    struct node_list_item *next = NULL;

    if(NULL == node_list) return NULL;
    current = node_list->head;

    while(current != NULL) {
        if(current->item->id == id) return current->item;
        next = current->next;
        current = next;
    }

    return NULL;
}

struct node *node_list_find_by_name(struct node_list *node_list, char *name)
{
    struct node_list_item *current = NULL;
    struct node_list_item *next = NULL;

    if(NULL == node_list) return NULL;
    current = node_list->head;

    while(current != NULL) {
        if(0 == strncpy(current->item->name, name, current->item->name_length)) {
            return current->item;
        }
        next = current->next;
        current = next;
    }

    return NULL;
}
