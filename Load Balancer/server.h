/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */
#ifndef SERVER_H_
#define SERVER_H_

#include "hashtable.h"

struct server_memory {
	unsigned int id;
	unsigned int hash;
	hashtable_t *ht;
};

typedef struct server_memory server_memory;

/** init_server_memory() -  Initializes the memory for a new server struct.
 * 							Make sure to check what is returned by malloc using DIE.
 * 							Use the linked list implementation from the lab.
 *
 * Return: pointer to the allocated server_memory struct.
 */
server_memory *init_server_memory();

/** free_server_memory() - Free the memory used by the server.
 *
 * @arg1: Server to free
 */
void free_server_memory(server_memory *server);

/** server_balance() - Balance a new server to take load from the next_server.
 *
 * @arg1: Server to get load
 * @arg2: Server to give load
 */
void server_balance(server_memory *server, server_memory *next_server);

/** server_remove() - Delete a server and give its load to the next_server.
 *
 * @arg1: Server to be deleted
 * @arg2: Server to get load from the deleted server
 */
void server_remove(server_memory *deleted_server, server_memory *new_server);

#endif /* SERVER_H_ */
