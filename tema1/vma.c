#include "vma.h"
#include "functions.h"
#include "list.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Functia aloca arena si creeaza lista de blocuri.
*/
arena_t *alloc_arena(const uint64_t size)
{
	arena_t *arena = (arena_t *)malloc(sizeof(*arena));
	DIE(!arena, "malloc failed");

	arena->arena_size = size;
	arena->alloc_list = dll_create();

	return arena;
}

/*
Functia dealoca arena prin apeluri succesive a functiei free_block.
*/
void dealloc_arena(arena_t *arena)
{
	dll_node_t *node = arena->alloc_list->head;
	uint64_t sz_arena_list = arena->alloc_list->size;

	for (uint64_t i = 0 ; i < sz_arena_list ; ++i) {
		node = arena->alloc_list->head;

		list_t *mini_list = (list_t *)((block_t *)node->data)->miniblock_list;
		miniblock_t *mini_bl = ((miniblock_t *)mini_list->head->data);
		uint64_t sz_mini_list = mini_list->size;

		for (uint64_t j = 0 ; j < sz_mini_list ; ++j) {
			mini_bl = ((miniblock_t *)mini_list->head->data);
			free_block(arena, mini_bl->start_address);
		}
	}

	free(arena->alloc_list);
	free(arena);
}

/*
Functia verifica pozitia noii zone de alocat si apeleaza functia necesara.
*/
void alloc_block(arena_t *arena, const uint64_t address, const uint64_t size)
{
	dll_node_t *node = arena->alloc_list->head;
	uint64_t pos = 0, sz_arena_list = arena->alloc_list->size;
	int check = 0;

	if (address < arena->arena_size && address + size > arena->arena_size) {
		printf("The end address is past the size of the arena\n");
		return;
	} else if (address + size > arena->arena_size) {
		printf("The allocated address is outside the size of arena\n");
		return;
	}

	if (!node) {
		create_block(arena, pos, address, size);
		return;
	}

	block_t *block = ((block_t *)node->data);
	while (sz_arena_list-- && block->start_address + block->size < address) {
		node = node->next;
		block = ((block_t *)node->data);
		pos++;
	}

	check = check_position(block->start_address, block->size,
						   address, size);

	if (block->start_address + block->size == address) {
		block = ((block_t *)node->next->data);
		if (block->start_address == address + size)
			check = 3;
	}

	if (!check)
		printf("This zone was already allocated.\n");
	else if (check == 1)
		create_block(arena, pos, address, size);
	else if (check == -1)
		create_block(arena, pos, address, size);
	else if (check == 2)
		concatenate_block(arena, pos, address, size, 1);
	else if (check == -2)
		concatenate_block(arena, pos, address, size, 0);
	else if (check == 3)
		merge_block(arena, pos, address, size);
}

/*
Functia verifica unul din cele 4 cazuri posibile: adresa este a unui minibloc
aflat singur in lista de miniblocuri, adresa este a unui minibloc aflat la
inceputul sau sfarsitul listei sau aceasta apartine unui minibloc aflat in
interiorul listei, caz in care apeleaza functia split_block.
*/
void free_block(arena_t *arena, const uint64_t address)
{
	dll_node_t *node = arena->alloc_list->head, *node_block = NULL;
	uint64_t pos = 0, sz_block = 0, loc = 0;

	if (!arena->alloc_list->size) {
		printf("Invalid address for free.\n");
		return;
	}

	for (uint64_t i = 0 ; i < arena->alloc_list->size ; ++i) {
		if (((block_t *)node->data)->start_address >= address)
			break;
		node = node->next;
		loc++;
	}
	if (loc && ((block_t *)node->data)->start_address != address)
		node = node->prev;

	node_block = ((list_t *)(((block_t *)node->data)->miniblock_list))->head;
	sz_block = ((list_t *)(((block_t *)node->data)->miniblock_list))->size;

	for (uint64_t i = 0 ; i < sz_block ; ++i) {
		if (((miniblock_t *)node_block->data)->start_address >= address)
			break;
		node_block = node_block->next;
		pos++;
	}
	if (((miniblock_t *)node_block->data)->start_address != address) {
		printf("Invalid address for free.\n");
		return;
	}
	list_t *mini_list = ((block_t *)node->data)->miniblock_list;
	miniblock_t *mini_bl = ((miniblock_t *)mini_list->head->data);
	if (sz_block == 1) {
		/* stergem de tot blocul */

		free(mini_bl->rw_buffer);
		dll_free(&mini_list);
		free(dll_get_nth_node(arena->alloc_list, loc)->data);
		free(dll_remove_nth_node(arena->alloc_list, loc));
	} else if (!pos) {
		/* stergem primul minibloc */

		((block_t *)node->data)->start_address += mini_bl->size;
		free(mini_bl->rw_buffer);
		free(dll_get_nth_node(mini_list, 0)->data);
		free(dll_remove_nth_node(mini_list, 0));
	} else if (pos == sz_block - 1) {
		/* stergem ultimul minibloc*/

		mini_bl = dll_get_nth_node(mini_list, mini_list->size - 1)->data;
		((block_t *)node->data)->size -= mini_bl->size;
		free(mini_bl->rw_buffer);
		free(mini_bl);
		free(dll_remove_nth_node(mini_list, mini_list->size - 1));
	} else {
		/* spargem in 2 blocuri */

		split_block(arena, node, mini_list, pos, address, loc);
	}
}

/*
Functia afiseaza caracter cu caracter datele pana ajunge la sfarsitul
dimensiunii sau a blocului, verificand daca avem permisiunile necesare.
*/
void read(arena_t *arena, uint64_t address, uint64_t size)
{
	dll_node_t *node = arena->alloc_list->head, *node_block = NULL;
	uint64_t pos = 0, loc = 0;

	if (!arena->alloc_list->size || address + size > arena->arena_size) {
		printf("Invalid address for read.\n");
		return;
	}

	for (uint64_t i = 0 ; i < arena->alloc_list->size ; ++i) {
		if (((block_t *)node->data)->start_address >= address)
			break;
		node = node->next;
		loc++;
	}
	if (loc && ((block_t *)node->data)->start_address != address)
		node = node->prev;

	node_block = ((list_t *)(((block_t *)node->data)->miniblock_list))->head;
	uint64_t sz = ((list_t *)((block_t *)node->data)->miniblock_list)->size;

	for (uint64_t i = 0 ; i < sz; ++i) {
		if (((miniblock_t *)node_block->data)->start_address >= address)
			break;
		node_block = node_block->next;
		pos++;
	}
	if (pos && ((miniblock_t *)node_block->data)->start_address != address)
		node_block = node_block->prev;

	if (!check_perm(arena, address, size, 0b100)) {
		printf("Invalid permissions for read.\n");
		return;
	}
	if (get_block_size(arena, address) < size) {
		printf("Warning: size was bigger than the block size. Reading "
		"%lu characters.\n", get_block_size(arena, address));
	}
	if (((miniblock_t *)node_block->data)->size + ((miniblock_t *)node_block->data)->start_address < address) {
		printf("Invalid address for read.\n");
		return;
	}

	uint64_t cnt = address - ((miniblock_t *)node_block->data)->start_address;
	while (size--) {
		fwrite(((miniblock_t *)node_block->data)->rw_buffer + cnt, 1, 1, OUT);
		cnt++;
		if (cnt >= ((miniblock_t *)node_block->data)->size) {
			cnt = 0;
			sz = ((list_t *)((block_t *)node->data)->miniblock_list)->size;
			if (pos + 1 < sz) {
				node_block = node_block->next;
				pos++;
			} else {
				break;
			}
		}
	}
	printf("\n");
}

/*
Functia citeste de la stdin pana s-au citit "size" caractere si le scrie in
miniblocuri incepand de la adresa data, verificand daca avem perimisiunile
necesare sau daca nu am ajuns la finalul blocului.
*/
void write(arena_t *arena, const uint64_t address, uint64_t size,
		   int8_t *data)
{
	dll_node_t *node = arena->alloc_list->head, *node_block = NULL;
	uint64_t pos = -1, partial_size = strlen((char *)data);

	if (!arena->alloc_list->size) {
		printf("Invalid address for write.\n");
		char *buff = calloc(size * sizeof(char), 1);
		while (partial_size < size) {
			fgets((char *)buff, size - partial_size + 1, stdin);
			partial_size += strlen(buff);
		}
		free(buff);
		return;
	}
	for (uint64_t i = 0 ; i < arena->alloc_list->size ; ++i) {
		if (((block_t *)node->data)->start_address >= address)
			break;
		node = node->next;
	}
	if (((block_t *)node->data)->start_address != address)
		node = node->prev;

	node_block = ((list_t *)(((block_t *)node->data)->miniblock_list))->head;
	uint64_t sz = ((list_t *)((block_t *)node->data)->miniblock_list)->size;

	for (uint64_t i = 0 ; i < sz && pos++; ++i) {
		if (((miniblock_t *)node_block->data)->start_address >= address)
			break;
		node_block = node_block->next;
	}

	if (!check_perm(arena, address, size, 0b010))
		printf("Invalid permissions for write.\n");

	if (partial_size > size)
		partial_size = size;

	int8_t *all_data = calloc(size * sizeof(int8_t) + 1, 1);
	int8_t *buff = calloc(size * sizeof(int8_t) + 1, 1);
	strncpy((char *)all_data, (char *)data, partial_size);

	while (partial_size < size) {
		fgets((char *)buff, size - partial_size + 1, stdin);
		memcpy(all_data + partial_size, buff, strlen((char *)buff));
		partial_size += strlen((char *)buff);
	}
	if (partial_size > size)
		partial_size = size;

	if (address < ((miniblock_t *)node_block->data)->start_address || address >
		((miniblock_t *)node_block->data)->start_address +
		((miniblock_t *)node_block->data)->size) {
		printf("Invalid address for write.\n");
		free(all_data); free(buff);
		return;
	}

	if (get_block_size(arena, address) < size) {
		printf("Warning: size was bigger than the block size. Writing %lu "
		"characters.\n", get_block_size(arena, address));
	}

	int x = - ((miniblock_t *)node_block->data)->start_address + address;
	for (; pos < sz && size ; ++pos) {
		miniblock_t *mini = ((miniblock_t *)node_block->data);
		if (((miniblock_t *)node_block->data)->size < size) {
			strncpy(mini->rw_buffer + x, (char *)all_data, mini->size - x);
			size = size - ((miniblock_t *)node_block->data)->size + x;
			strncpy((char *)buff, (char *)all_data + mini->size, size);
			strncpy((char *)all_data, (char *)buff, size);
		} else {
			memcpy(mini->rw_buffer, all_data, size);
			size = 0;
		}
		x = 0;
		if (!size)
			break;
		node_block = node_block->next;
	}
	free(all_data); free(buff);
}

/*
Functia afiseaza continutul arenei.
*/
void pmap(const arena_t *arena)
{
	uint64_t alloc_mem = 0, alloc_miniblocks = 0;
	printf("Total memory: 0x%lX bytes\n", arena->arena_size);

	dll_node_t *node = arena->alloc_list->head;
	for (uint64_t i = 0 ; i < arena->alloc_list->size ; ++i) {
		alloc_miniblocks += ((list_t *)((block_t *)node->data)->
		miniblock_list)->size;
		alloc_mem += ((block_t *)(node->data))->size;
		node = node->next;
	}

	printf("Free memory: 0x%lX bytes\n", arena->arena_size - alloc_mem);
	printf("Number of allocated blocks: %u\n", arena->alloc_list->size);
	printf("Number of allocated miniblocks: %lu\n", alloc_miniblocks);

	node = arena->alloc_list->head;
	for (unsigned int i = 0 ; i < arena->alloc_list->size ; ++i) {
		printf("\nBlock %d begin\nZone: 0x%lX - 0x%lX\n", i + 1,
			   ((block_t *)(node->data))->start_address,
			   ((block_t *)(node->data))->start_address +
			   ((block_t *)(node->data))->size);

		dll_node_t *node_block = ((list_t *)(((block_t *)node->data)->
		miniblock_list))->head;
		uint64_t sz = ((list_t *)((block_t *)node->data)->miniblock_list)->size;

		for (uint64_t j = 0 ; j < sz ; ++j) {
			printf("Miniblock %ld:\t\t0x%lX\t\t-\t\t0x%lX\t\t| ", j + 1,
				   ((miniblock_t *)node_block->data)->start_address,
				   ((miniblock_t *)node_block->data)->start_address +
				   ((miniblock_t *)node_block->data)->size);

			print_perm(((miniblock_t *)node_block->data)->perm);

			node_block = node_block->next;
		}

		printf("Block %d end\n", i + 1);
		node = node->next;
	}
}

/*
Functia asigneaza permisiunea primita miniblocului care se afla la acea adresa.
*/
void mprotect(arena_t *arena, uint64_t address, int8_t permission)
{
	dll_node_t *curr = arena->alloc_list->head, *node;
	uint64_t loc = 0;

	if (arena->alloc_list->size == 0) {
		printf("Invalid address for mprotect.\n");
		return;
	}

	for (unsigned int i = 0 ; i < arena->alloc_list->size ; ++i) {
		if (((block_t *)curr->data)->start_address >= address)
			break;
		curr = curr->next;
		loc++;
	}

	node = ((list_t *)((block_t *)curr->data)->miniblock_list)->head;
	uint64_t sz = ((list_t *)((block_t *)curr->data)->miniblock_list)->size;

	for (unsigned int i = 0 ; i < sz ; ++i) {
		if (((miniblock_t *)node->data)->start_address == address)
			break;
		node = node->next;
	}

	if (((miniblock_t *)node->data)->start_address != address) {
		printf("Invalid address for mprotect.\n");
		return;
	}

	((miniblock_t *)node->data)->perm = permission;
}
