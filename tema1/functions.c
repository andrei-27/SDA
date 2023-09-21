#include "functions.h"
#include <stdint.h>

/*
Functia creeaza un bloc si il adauga la lista de blocuri din arena.
*/
void create_block(arena_t *arena, uint64_t pos, const uint64_t address,
				  const uint64_t size)
{
	dll_add_nth_node(arena->alloc_list, pos);
	dll_node_t *node = dll_get_nth_node(arena->alloc_list, pos);

	node->data = malloc(sizeof(block_t));
	DIE(!node->data, "malloc failed");

	((block_t *)node->data)->size = size;
	((block_t *)node->data)->start_address = address;
	((block_t *)node->data)->miniblock_list = dll_create();

	list_t *mini_list = ((block_t *)node->data)->miniblock_list;

	dll_add_nth_node(mini_list, 0);
	mini_list->head->data = malloc(sizeof(miniblock_t));
	DIE(!mini_list->head->data, "malloc failed");

	miniblock_t *miniblock = (miniblock_t *)(mini_list->head->data);

	miniblock->size = size;
	miniblock->start_address = address;
	miniblock->perm = 0b110; // RW-
	miniblock->rw_buffer = calloc(miniblock->size, 1);
	DIE(!miniblock->rw_buffer, "malloc failed");
}

/*
Functia creeaza si concateneaza un minibloc si il adauga la lista de miniblocuri
din blocul corespunzator. Primeste ca parametru side: adaugarea noului minibloc
se realizeaza la inceputul listei (side == 0) sau la sfarsitul acesteia
(side == 1).
*/
void concatenate_block(arena_t *arena, uint64_t pos, const uint64_t address,
					   const uint64_t size, size_t side)
{
	dll_node_t *node = dll_get_nth_node(arena->alloc_list, pos);
	block_t *my_block = ((block_t *)node->data);
	list_t *mini_list = ((list_t *)my_block->miniblock_list);

	my_block->size += size;

	if (side) {
		dll_add_nth_node(mini_list, mini_list->size);
		dll_node_t *node = (mini_list->head->prev);

		node->data = malloc(sizeof(miniblock_t));

		DIE(!node->data, "malloc failed");

		miniblock_t *miniblock = (miniblock_t *)(node->data);

		miniblock->size = size;
		miniblock->start_address = address;
		miniblock->perm = 0b110; // RW-
		miniblock->rw_buffer = calloc(miniblock->size, 1);

		DIE(!miniblock->rw_buffer, "malloc failed");
	} else {
		my_block->start_address = address;

		dll_add_nth_node(my_block->miniblock_list, 0);
		dll_node_t *node = (mini_list->head);

		node->data = malloc(sizeof(miniblock_t));

		DIE(!node->data, "malloc failed");

		miniblock_t *miniblock = (miniblock_t *)(node->data);

		miniblock->size = size;
		miniblock->start_address = address;
		miniblock->perm = 0b110; // RW-
		miniblock->rw_buffer = calloc(miniblock->size, 1);

		DIE(!miniblock->rw_buffer, "malloc failed");
	}
}

/*
Functia creeaza un minibloc si il adauga intre alte 2 blocuri, formand un singur
bloc. Realizeaza acest lucru prin apelarea functiei de concatenare la sfarsitul
primului bloc, iar apoi mareste dimensiunea primului bloc. Dupa ce sunt
rezolvate legaturile dintr cele 2 liste de miniblocuri ca in final sa formeze o
singura lista este eliberata memoria blocului ce a fost eliminat, fara a altera
lista de miniblocuri.
*/
void merge_block(arena_t *arena, uint64_t pos, const uint64_t address,
				 const uint64_t size)
{
	concatenate_block(arena, pos, address, size, 1);
	dll_node_t *node = dll_get_nth_node(arena->alloc_list, pos);

	block_t *my_block = ((block_t *)node->data);
	my_block->size += ((block_t *)node->next->data)->size;

	list_t *left_list = my_block->miniblock_list;
	list_t *right_list = ((block_t *)node->next->data)->miniblock_list;
	dll_node_t *node_left = left_list->head->prev;
	dll_node_t *node_right = right_list->head;

	node_left->next = node_right;
	node_right->prev->next = left_list->head;
	left_list->head->prev = node_right->prev;
	node_right->prev = node_left;

	left_list->size += right_list->size;
	free(((block_t *)node->next->data)->miniblock_list);
	free(node->next->data);
	free(dll_remove_nth_node(arena->alloc_list, pos + 1));
}

/*
Functia este apelata free_block in cazul in care trebuie sters un minibloc aflat
in interiorul unei liste de miniblocuri. Spargem lista de miniblocuri in 2 liste
independente si eliminam nodul (miniblocul) care trebuie sters. Este alocat un
nou bloc, iar lista lui devine partea din lista de dupa nodul eliminat.
*/
void split_block(arena_t *arena, dll_node_t *node, list_t *mini_list,
				 uint64_t pos, uint64_t address, uint64_t loc)
{
	block_t *block = node->data;
	node = mini_list->head;

	for (pos = 0 ; pos < mini_list->size ; ++pos) {
		if (((miniblock_t *)node->data)->start_address == address)
			break;
		node = node->next;
	}

	block->size -= ((miniblock_t *)node->data)->size;
	uint64_t sz_mini_list = mini_list->size;
	uint64_t new_start = ((miniblock_t *)node->next->data)->start_address;
	miniblock_t *last = dll_get_nth_node(mini_list, sz_mini_list - 1)->data;
	uint64_t new_size = last->start_address + last->size -
	(((miniblock_t *)node->next->data)->start_address);

	block->size -= new_size;
	create_block(arena, loc, new_start, new_size);
	free(((miniblock_t *)dll_get_nth_node(mini_list, pos)->data)->rw_buffer);
	free(dll_get_nth_node(mini_list, pos)->data);
	free(dll_remove_nth_node(mini_list, pos));

	node = dll_get_nth_node(mini_list, pos - 1);
	dll_node_t *new_node = node->next;
	dll_node_t *tmp = dll_get_nth_node(mini_list, sz_mini_list - 2);
	dll_node_t *neww_list = dll_get_nth_node(arena->alloc_list, loc);
	list_t *new_list = ((block_t *)neww_list->data)->miniblock_list;

	node->next = mini_list->head;
	mini_list->head->prev = node;
	mini_list->size = pos;

	free(((miniblock_t *)new_list->head->data)->rw_buffer);
	free(new_list->head->data);
	free(new_list->head);

	new_list->head = new_node;
	new_node->prev = tmp;
	tmp->next = new_node;
	new_list->size = sz_mini_list - pos - 1;
}

/*
Functia verifica in ce caz ne aflam cand adaugam un bloc: blocul este
independent (+-1), este adiacent cu alt bloc (+-2) sau este invalid (0).
*/
int check_position(uint64_t b_st, size_t b_sz, uint64_t new_st, size_t new_sz)
{
	uint64_t b_end = b_st + b_sz;
	uint64_t new_end = new_st + new_sz;
	if (new_st >= b_st && new_st < b_end)
		return 0; // invalid
	if (new_end > b_st && new_end <= b_end)
		return 0; // invalid
	if (new_st <= b_st && new_end >= b_end)
		return 0; // invalid
	if (new_st >= b_st && new_end <= b_end)
		return 0; // invalid
	if (new_st == b_end)
		return 2; // concatenate after the block
	if (new_end == b_st)
		return -2; // concatenate before the block
	if (new_st > b_end)
		return 1; // add after the block
	if (new_end < b_st)
		return -1;	// add before the block
	return 0;
}

/*
Functia actualizeaza dimensiunea blocului in functie de ultimul minibloc din
lista.
*/
void refresh_block_size(arena_t *arena)
{
	dll_node_t *node = arena->alloc_list->head;

	for (unsigned int i = 0 ; i < arena->alloc_list->size ; ++i) {
		dll_node_t *node_block = ((list_t *)(((block_t *)node->data)->
		miniblock_list))->head->prev;

		((block_t *)node->data)->size = ((miniblock_t *)node_block->data)->
		start_address + ((miniblock_t *)node_block->data)->size -
		((block_t *)node->data)->start_address;
		node = node->next;
	}
}

/*
Functia returneaza lungimea de la address pana la sfarsitul blocului in care se
afla adresa primita.
*/
uint64_t get_block_size(arena_t *arena, uint64_t address)
{
	dll_node_t *node = arena->alloc_list->head, *node_block = NULL;
	uint64_t block_size = 0, pos = 0, loc = 0;

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

	if (pos && ((miniblock_t *)node_block->data)->start_address != address) {
		node_block = node_block->prev;
		pos--;
	}
	pos++;

	uint64_t start = ((miniblock_t *)node_block->data)->start_address;
	uint64_t size_node = ((miniblock_t *)node_block->data)->size;
	block_size = start + size_node - address;

	for (; pos < sz; ++pos) {
		block_size += ((miniblock_t *)node_block->data)->size;
		node_block = node_block->next;
	}
	return block_size;
}

/*
Functia returneaza 1 daca toate miniblocurile din intervalul [address, address +
size] au permisiunea primita ca parametru sau 0 in caz contrar. Verificarea se
face printr-un si logic dintre permisiunea miniblocului si cea primita.
*/
uint64_t check_perm(arena_t *arena, uint64_t address, uint64_t size,
					int8_t perm)
{
	dll_node_t *node = arena->alloc_list->head, *node_block = NULL;
	uint64_t pos = 0, loc = 0, check = 1, stop = address + size;

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

	if (pos && ((miniblock_t *)node_block->data)->start_address != address) {
		node_block = node_block->prev;
		pos--;
	}

	for (; pos < sz ; ++pos) {
		if (((miniblock_t *)node_block->data)->start_address >= stop)
			break;
		if ((((miniblock_t *)node_block->data)->perm & perm) != perm) {
			check = 0;
			break;
		}
		node_block = node_block->next;
	}
	return check;
}

/*
Functia este apelata de pmap pentru a afisa ppermisiunile unui minibloc.
*/
void print_perm(int8_t perm)
{
	if (perm == 0b111)
		printf("RWX\n");
	else if (perm == 0b110)
		printf("RW-\n");
	else if (perm == 0b101)
		printf("R-X\n");
	else if (perm == 0b100)
		printf("R--\n");
	else if (perm == 0b011)
		printf("-WE\n");
	else if (perm == 0b010)
		printf("-W-\n");
	else if (perm == 0b001)
		printf("--E\n");
	else if (perm == 0b000)
		printf("---\n");
}
