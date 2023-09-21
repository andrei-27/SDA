/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#ifndef TRIE_H
#define TRIE_H

#define ALPHABET_SIZE 26
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"

#define DIE(assertion, call_description)                                       \
	do {                                                                       \
		if (assertion) {                                                       \
			fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
			perror(call_description);                                          \
			exit(errno);                                                       \
		}                                                                      \
	} while (0)

typedef struct trie_node_t trie_node_t;
struct trie_node_t {
	int word_occurance;
	trie_node_t **children;
	int n_children;
};

typedef struct trie_t trie_t;
struct trie_t {
	trie_node_t *root;
	int size;
	int alphabet_size;
	char *alphabet;
};

trie_node_t *trie_create_node(trie_t *trie);

trie_t *trie_create(int alphabet_size, char *alphabet);

void trie_insert(trie_t *trie, char *key);

int trie_search(trie_t *trie, char *key);

void trie_free_subtrie(trie_t *trie, trie_node_t *node);

void trie_remove(trie_t *trie, char *key);

void trie_free(trie_t **ptr_trie);

#endif  // TRIE_H
