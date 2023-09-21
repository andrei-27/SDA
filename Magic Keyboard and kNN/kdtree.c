/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#include "kdtree.h"

/*
Functia creaza un nod.
*/
kd_node_t *kd_node_create(int *coord, int k)
{
	kd_node_t *node;

	node = malloc(sizeof(*node));
	DIE(!node, "node malloc");

	node->left = NULL;
	node->right = NULL;

	node->coord = malloc(sizeof(int) * k);
	DIE(!node->coord, "node->coord malloc");
	memcpy(node->coord, coord, sizeof(int) * k);

	return node;
}

/*
Functia insereaza un nod, verificand la fiecare nivel coordonatele
corespunzatoare.
*/
kd_node_t *kd_tree_insert(kd_node_t *root, int *coord, int k, int depth)
{
	if (!root)
		return kd_node_create(coord, k);

	int curr_depth = depth % k;

	if (coord[curr_depth] < root->coord[curr_depth])
		root->left = kd_tree_insert(root->left, coord, k, ++depth);
	else
		root->right = kd_tree_insert(root->right, coord, k, ++depth);

	return root;
}

/*
Functia elibereaza memoria tree-ului.
*/
void kd_free(kd_node_t *root)
{
	if (root) {
		kd_free(root->left);
		kd_free(root->right);
		free(root->coord);
		free(root);
	}
}
