#pragma once
#include "vma.h"

list_t *dll_create(void);
dll_node_t *dll_get_nth_node(list_t *list, unsigned int n);
void dll_add_nth_node(list_t *list, unsigned int n);
dll_node_t *dll_remove_nth_node(list_t *list, unsigned int n);
void dll_free(list_t **list);
