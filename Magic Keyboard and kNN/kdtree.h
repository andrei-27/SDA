/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#ifndef KDTREE_H
#define KDTREE_H

#include "kNN.h"

typedef struct kd_node_t kd_node_t;
struct kd_node_t {
	kd_node_t *left;
	kd_node_t *right;
	int *coord;
};

kd_node_t *kd_node_create(int *coord, int k);

kd_node_t *kd_tree_insert(kd_node_t *root, int *coord, int k, int depth);

void kd_free(kd_node_t *root);

#endif  // KDTREE_H
