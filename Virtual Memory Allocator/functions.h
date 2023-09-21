#pragma once
#include "vma.h"
#include "list.h"

void create_block(arena_t *arena, uint64_t pos, const uint64_t address,
				  const uint64_t size);
void concatenate_block(arena_t *arena, uint64_t pos, const uint64_t address,
					   const uint64_t size, size_t side);
void merge_block(arena_t *arena, uint64_t pos, const uint64_t address,
				 const uint64_t size);
void split_block(arena_t *arena, dll_node_t *node, list_t *mini_list,
				 uint64_t pos, uint64_t address, uint64_t loc);

int check_position(uint64_t b_st, size_t b_sz, uint64_t new_st, size_t new_sz);
void refresh_block_size(arena_t *arena);
uint64_t get_block_size(arena_t *arena, uint64_t address);
uint64_t check_perm(arena_t *arena, uint64_t address, uint64_t size,
					int8_t perm);

void print_perm(int8_t perm);
