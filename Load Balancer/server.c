/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "server.h"

#define HMAX_SERVER 100

server_memory *init_server_memory()
{
	struct server_memory *server = malloc(sizeof(server_memory));
	server->id = 0;
	server->hash = 0;
	server->ht = ht_create(HMAX_SERVER, hash_function_key,
						   compare_function_strings, key_val_free_function);

	return server;
}

void free_server_memory(server_memory *server) {
	ht_free(server->ht);
	server->ht = NULL;
}

void server_balance(server_memory *server, server_memory *next_server)
{
	for (unsigned int i = 0; i < next_server->ht->hmax; i++) {
		linked_list_t* list = next_server->ht->buckets[i];
        ll_node_t* current_node = list->head;

        while (current_node != NULL) {
			char *key = ((info*)(current_node->data))->key;
			unsigned int h = hash_function_key(key);

			if (server->hash > next_server->hash && h < server->hash) {
				char *value = ht_get(next_server->ht, key);
				ht_put(server->ht, key, strlen(key) + 1, value, strlen(value) + 1);
			} else if (server->hash < next_server->hash) {
				if (h < server->hash || h > next_server->hash) {
					char *value = ht_get(next_server->ht, key);
					ht_put(server->ht, key, strlen(key) + 1, value, strlen(value) + 1);
				}
			}

            current_node = current_node->next;
        }
    }
}

void server_remove(server_memory *deleted_server, server_memory *new_server)
{
	for (unsigned int i = 0; i < deleted_server->ht->hmax; i++) {
		linked_list_t* list = deleted_server->ht->buckets[i];
        ll_node_t* current_node = list->head;

        while (current_node != NULL) {
			char *key = ((info*)(current_node->data))->key;
			char *value = ht_get(deleted_server->ht, key);

			ht_put(new_server->ht, key, strlen(key) + 1, value, strlen(value) + 1);

            current_node = current_node->next;
        }
    }

	free_server_memory(deleted_server);
	free(deleted_server);
}
