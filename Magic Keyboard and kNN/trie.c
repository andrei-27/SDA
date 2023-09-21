/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#include "trie.h"
#include "mk.h"

/*
Functia creaza un nod.
*/
trie_node_t *trie_create_node(trie_t *trie)
{
	trie_node_t *node = malloc(sizeof(trie_node_t));
	DIE(!node, "malloc node");

	node->word_occurance = 0;
	node->children = calloc(trie->alphabet_size, sizeof(trie_node_t *));
	DIE(!node->children, "calloc children");
	node->n_children = 0;

	return node;
}

/*
Functia creaza un trie.
*/
trie_t *trie_create(int alphabet_size, char *alphabet)
{
	trie_t *trie = malloc(sizeof(trie_t));
	DIE(!trie, "malloc trie");

	trie->size = 0;
	trie->alphabet_size = alphabet_size;
	trie->alphabet = alphabet;

	trie->root = trie_create_node(trie);

	return trie;
}

/*
Functia insereaza un cuvant in trie.
*/
void trie_insert(trie_t *trie, char *key)
{
	if (key[0] == '\0') {
		trie->root->word_occurance++;
		return;
	}

	trie_node_t *current = trie->root;
	int i = 0;

	while (key[i] != '\0') {
		int letter = key[i] - 'a';

		if (!current->children[letter]) {
			current->children[letter] = trie_create_node(trie);
			++current->n_children;
		}

		current = current->children[letter];
		++i;
	}

	current->word_occurance++;
	++trie->size;
}

/*
Functia cauta un cuvant in trie.
*/
int trie_search(trie_t *trie, char *key)
{
	if (key[0] == '\0')
		return 1;

	trie_node_t *current = trie->root;
	int i = 0;

	while (key[i] != '\0') {
		int letter = key[i] - 'a';

		if (!current->children[letter])
			return 0; // not found

		current = current->children[letter];
		++i;
	}

	return 1; // found
}

/*
Functia elibereaza recursiv memoria alocata nodurilor.
*/
void trie_free_subtrie(trie_t *trie, trie_node_t *node)
{
	if (!node)
		return;

	for (int i = 0; i < trie->alphabet_size && node->n_children; ++i) {
		if (!node->children[i])
			continue;

		trie_free_subtrie(trie, node->children[i]);
		--node->n_children;
	}

	free(node->children);
	free(node);
}

/*
Functia sterge un cuvant din trie.
*/
void trie_remove(trie_t *trie, char *key)
{
	if (!trie_search(trie, key))
		return;

	if (key[0] == '\0') {
		trie->root->word_occurance = 0;
		--trie->size;
		return;
	}

	trie_node_t *current = trie->root;
	trie_node_t *parent = trie->root;
	int parent_letter = (key[0] - 'a');
	int i = 0;

	while (key[i] != '\0') {
		int letter_pos = key[i] - 'a';

		if (!current->children[letter_pos])
			return;

		if (current->n_children > 1 || current->word_occurance) {
			parent = current;
			parent_letter = letter_pos;
		}

		current = current->children[letter_pos];

		++i;
	}

	current->word_occurance = 0;
	if (!current->n_children) {
		trie_free_subtrie(trie, parent->children[parent_letter]);
		parent->children[parent_letter] = NULL;
		--parent->n_children;
	}

	--trie->size;
}

/*
Functia elibereaza memoria alocata trie-ului.
*/
void trie_free(trie_t **ptr_trie)
{
	trie_free_subtrie(*ptr_trie, (*ptr_trie)->root);
	free(*ptr_trie);
}
