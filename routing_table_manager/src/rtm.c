//
// Created by Alexandra Zaharia on 15/02/19.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "input.h"
#include "rtm.h"

#define MALLOC_ERROR(func, msg) fprintf(stderr, "%s: %s\n", (func), (msg)) // TODO move elsewhere


// Creates a new routing table.
RoutingTable* routing_table_create()
{
    return linked_list_create();
}


// Frees a routing table
void routing_table_free(RoutingTable* rt)
{
    for (DNode *node = rt->head; node; node = node->next) {
        msg_body_t *record = node->data;
        if (record) free(record);
    }
    rt->free(rt);
}


// Creates a new routing record.
msg_body_t *routing_record_create()
{
    msg_body_t *record = malloc(sizeof(msg_body_t));
    if (!record) {
        MALLOC_ERROR(__func__, "Insufficient memory for routing record");
        return NULL;
    }
    return record;
}


// Creates an operation on a routing table from the provided operation code `op_code' and `record'.
sync_msg_t *rtm_operation_create(OP_CODE op_code, msg_body_t *record)
{
    sync_msg_t *operation = malloc(sizeof(sync_msg_t));
    if (!operation) {
        MALLOC_ERROR(__func__, "Insufficient memory for routing table operation");
        return NULL;
    }
    operation->op_code  = op_code;
    operation->msg_body = *record;
    return operation;
}


// Inserts a new routing record into the routing table. Returns 0 on success and -1 on failure.
int routing_table_insert(RoutingTable* rt, msg_body_t* record)
{
    return rt->insert_end(rt, record);
}


/*
 * Updates a routing record with a new gateway IP and a new outgoing interface. Returns 0 on success
 * and -1 on failure.
 */
int routing_table_update(RoutingTable* rt, msg_body_t* record)
{
    if (!routing_table_contains_dst_subnet(rt, record->destination, record->mask)) return -1;

    for (DNode *node = rt->head; node; node = node->next) {
        msg_body_t *current = node->data;
        if (!strcmp(current->destination, record->destination) && current->mask == record->mask) {
            strncpy(current->gateway_ip, record->gateway_ip, IP_ADDR_LEN);
            strncpy(current->oif, record->oif, OIF_LEN);
            return 0;
        }
    }

    return -1;
}


// Deletes a routing record. Returns 0 on success and -1 on failure.
int routing_table_delete(RoutingTable* rt, msg_body_t* record)
{
    return 0;
}


// Displays all records in the routing table to stdout.
void routing_table_print(RoutingTable* rt)
{

}


// Checkes whether the given destination IP exists in the routing table.
bool routing_table_contains_dst(RoutingTable *rt, char *destination)
{
    for (DNode *node = rt->head; node; node = node->next) {
        msg_body_t *record = node->data;
        if (strncmp(destination, record->destination, strlen(record->destination)) == 0)
            return true;
    }
    return false;
}


// Checks whether the given destination IP and subnet mask exist in the routing table.
bool routing_table_contains_dst_subnet(RoutingTable *rt, char *destination, u16 mask)
{
    for (DNode *node = rt->head; node; node = node->next) {
        msg_body_t *record = node->data;
        if (strncmp(destination, record->destination, strlen(record->destination)) == 0
                && mask == record->mask)
            return true;
    }
    return false;
}
