/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#ifndef MK_H
#define MK_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

#define MAX_WORD 256

void dfs_autocorrect(trie_node_t *node, char *key, char *current_word,
					 int differences, int max_diff, size_t length,
					 int *words_found);

void trie_autocorrect(trie_t *trie, char *key, int k);

void trie_load(trie_t *trie, char *path);

void dfs_autocomplete_shortest(trie_node_t *node, char *word,
							   char *shortest_found, int length);

void dfs_autocomplete_freq(trie_node_t *node, char *word, char *most_freq,
						   int *max_freq, int length);

void complete_smallest(trie_t *trie, char *key);

void complete_shortest(trie_t *trie, char *key);

void complete_freq(trie_t *trie, char *key);

void trie_autocomplete(trie_t *trie, char *key, int nr_crit);

#endif  // MK_H
