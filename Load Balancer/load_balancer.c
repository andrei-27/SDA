/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "load_balancer.h"
#include "server.h"
#include "utils.h"

#define COPIES 3
#define FACTOR 100000

struct load_balancer {
	int nr_servers;
	server_memory **arr_servers;
};

unsigned int hash_function_servers(void *a) {
	unsigned int uint_a = *((unsigned int *)a);

	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
	uint_a = (uint_a >> 16u) ^ uint_a;
	return uint_a;
}

unsigned int hash_function_key(void *a) {
	unsigned char *puchar_a = (unsigned char *)a;
	unsigned int hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

load_balancer *init_load_balancer() {
	struct load_balancer *main_lb = malloc(sizeof(load_balancer));
	DIE(!main_lb, "malloc failed");

	main_lb->nr_servers = 0;
	main_lb->arr_servers = NULL;

	return main_lb;
}

void loader_add_server(load_balancer *main, int server_id) {
	main->nr_servers += 1;

	server_memory **tmp = realloc(main->arr_servers, main->nr_servers * COPIES *
								sizeof(server_memory));
	DIE(!tmp, "malloc failed");
	main->arr_servers = tmp;

	int pos = 0, nr_serv = main->nr_servers;

	for (int replica_id = 0 ; replica_id < COPIES ; ++replica_id) {
		unsigned int sticker = replica_id * FACTOR + server_id;

		struct server_memory *new_server = init_server_memory();
		new_server->id = sticker;
		new_server->hash = hash_function_servers(&sticker);

		for (pos = 0 ; pos < nr_serv * COPIES - COPIES + replica_id ; ++pos) {
			if (new_server->hash < main->arr_servers[pos]->hash)
				break;
		}

		for (int j = nr_serv * COPIES - COPIES + replica_id ; j > pos ; --j) {
			main->arr_servers[j] = main->arr_servers[j - 1];
		}

		main->arr_servers[pos] = new_server;

		if (main->nr_servers > 1) {
			server_memory *next_server;
			if (pos >= main->nr_servers * COPIES - COPIES + replica_id)
				next_server = main->arr_servers[0];
			else
				next_server = main->arr_servers[pos + 1];

			server_balance(new_server, next_server);
		}
	}
}

void loader_remove_server(load_balancer *main, int server_id) {
	int pos = 0;

	for (int replica_id = 0 ; replica_id < COPIES ; ++replica_id) {
		unsigned int sticker = replica_id * FACTOR + server_id;
		unsigned int h = hash_function_servers(&sticker);

		server_memory *deleted_server = NULL, *new_server = NULL;

		for (pos = 0 ; pos < main->nr_servers * COPIES ; ++pos) {
			if (main->arr_servers[pos]->hash == h) {
				deleted_server = main->arr_servers[pos];
				for (int k = pos ; k < main->nr_servers * COPIES - 1; ++k)
					main->arr_servers[k] = main->arr_servers[k + 1];
				break;
			}
		}

		if (pos >= main->nr_servers * COPIES - replica_id - 1) {
			new_server = main->arr_servers[0];
		}
		else
			new_server = main->arr_servers[pos];

		server_remove(deleted_server, new_server);
	}

	main->nr_servers -= 1;
	main->arr_servers = realloc(main->arr_servers, main->nr_servers * COPIES *
								sizeof(server_memory));
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id) {
	unsigned int h = hash_function_key(key), pos;
	server_memory *server = NULL;

	for (pos = 0 ; pos < main->nr_servers * COPIES ; ++pos) {
		if (h < main->arr_servers[pos]->hash)
			break;
	}

	if (pos >= main->nr_servers * COPIES) {
		(*server_id) = main->arr_servers[0]->id % FACTOR;
		server = main->arr_servers[0];
	} else  {
		(*server_id) = main->arr_servers[pos]->id % FACTOR;
		server = main->arr_servers[pos];
	}

	ht_put(server->ht, key, strlen(key) + 1, value, strlen(value) + 1);
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
	unsigned h = hash_function_key(key);
	int pos;
	server_memory *server = NULL;

	for (pos = 0 ; pos < main->nr_servers * COPIES ; ++pos) {
		if (main->arr_servers[pos]->hash > h)
			break;
	}

	if (pos >= main->nr_servers * COPIES) {
		(*server_id) = main->arr_servers[0]->id % FACTOR;
		server = main->arr_servers[0];
	} else  {
		(*server_id) = main->arr_servers[pos]->id % FACTOR;
		server = main->arr_servers[pos];
	}

	return ht_get(server->ht, key);
}

void free_load_balancer(load_balancer *main) {
	for (int pos = 0 ; pos < main->nr_servers * COPIES ; ++pos) {
		free_server_memory(main->arr_servers[pos]);
		free(main->arr_servers[pos]);
	}

	free(main->arr_servers);
	free(main);
}
