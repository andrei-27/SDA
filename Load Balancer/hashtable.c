/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "utils.h"
#include "load_balancer.h"


linked_list_t *ll_create(unsigned int data_size)
{
	linked_list_t* ll;

	ll = malloc(sizeof(*ll));
	DIE(!ll, "malloc failed");

	ll->head = NULL;
	ll->data_size = data_size;
	ll->size = 0;

	return ll;
}

void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
	ll_node_t *prev, *curr;
	ll_node_t* new_node;

	if (!list) {
		return;
	}

	if (n > list->size) {
		n = list->size;
	}

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	new_node = malloc(sizeof(*new_node));
	DIE(!new_node, "malloc failed");
	new_node->data = malloc(list->data_size);
	DIE(!new_node->data, "malloc failed");
	memcpy(new_node->data, new_data, list->data_size);

	new_node->next = curr;
	if (prev == NULL) {
		list->head = new_node;
	} else {
		prev->next = new_node;
	}

	list->size++;
}


ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
	ll_node_t *prev, *curr;

	if (!list || !list->head) {
		return NULL;
	}

	if (n > list->size - 1) {
		n = list->size - 1;
	}

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	if (prev == NULL) {
		list->head = curr;
	} else {
		prev->next = curr->next;
	}

	list->size--;

	return curr;
}

unsigned int ll_get_size(linked_list_t* list)
{
	 if (!list) {
		return -1;
	}

	return list->size;
}


void ll_free(linked_list_t** pp_list)
{
	if (!pp_list || !*pp_list) {
		return;
	}

	while (ll_get_size(*pp_list) > 0) {
		ll_remove_nth_node(*pp_list, 0);
	}

	free(*pp_list);
	*pp_list = NULL;
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

void key_val_free_function(void *data) {
	info *node = (info *)data;
	free(node->key);
	free(node->value);
	free(data);
}

hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*))
{
	hashtable_t *ht = (hashtable_t *)malloc(sizeof(hashtable_t));
	DIE(!ht, "malloc failed");
	ht->hmax = hmax;
	ht->size = 0;
	ht->hash_function = hash_function;
	ht->compare_function = compare_function;
	ht->key_val_free_function = key_val_free_function;

	ht->buckets = (linked_list_t **)malloc(hmax * sizeof(linked_list_t*));

	for (unsigned int i = 0 ; i < hmax ; ++i) {
		ht->buckets[i] = ll_create(sizeof(info));
	}

	return ht;
}

int ht_has_key(hashtable_t *ht, void *key)
{
	if (ht == NULL || ht->size == 0)
		return 0;

	unsigned int index = ht->hash_function(key) % ht->hmax;

	ll_node_t *node = ht->buckets[index]->head;

	while (node) {
		if (!ht->compare_function(((info*)node->data)->key, key))
			return 1;
		node = node->next;
	}

	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
	if (ht == NULL)
		return NULL;

	if (ht->size == 0)
		return NULL;

	unsigned int index = ht->hash_function(key) % ht->hmax;

	ll_node_t *node = ht->buckets[index]->head;

	while (node) {
		if (!ht->compare_function(((info*)node->data)->key, key))
			return ((info*)node->data)->value;
		node = node->next;
	}

	return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	if (!ht)
		return;

	unsigned int index = ht->hash_function(key) % ht->hmax;

	ll_node_t *node = ht->buckets[index]->head;

	while (node) {
		if (!ht->compare_function(((info*)node->data)->key, key)) {
			free(((info*)node->data)->value);
			((info*)node->data)->value = malloc(value_size);
			memcpy(((info*)node->data)->value, value, value_size);
			return;
		}
		node = node->next;
	}

	info *new_info = (info *)malloc(sizeof(info));
	DIE(!new_info, "malloc failed");
	new_info->key = malloc(key_size);
	DIE(!new_info->key, "malloc failed");
	new_info->value = malloc(value_size);
	DIE(!new_info->value, "malloc failed");

	memcpy(new_info->value, value, value_size);
	memcpy(new_info->key, key, key_size);

	ll_add_nth_node(ht->buckets[index], 0, new_info);
	free(new_info);
	ht->size++;

	return;
}

void ht_free(hashtable_t *ht)
{
	if (!ht)
		return;

	ll_node_t *node;

	for (unsigned int i = 0 ; i < ht->hmax ; ++i) {
		node = ht->buckets[i]->head;
		for (unsigned int j = 0 ; j < ht->buckets[i]->size ; ++j) {
			ll_node_t *cpy = node;
			node = node->next;
			key_val_free_function(cpy->data);
			free(cpy);
		}
		ll_free(&ht->buckets[i]);
	}

	free(ht->buckets);
	free(ht);

	return;
}
