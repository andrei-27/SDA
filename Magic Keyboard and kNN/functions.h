/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "kNN.h"
#include "kdtree.h"

int distance_squared(int *coord1, int *coord2, int k);

kd_node_t *closest(int *target_coord, kd_node_t *node1, kd_node_t *node2,
				   int k);

void store_knn(int *target_coord, int *coord, int k, point_t arr[],
			   int *nr_pts);

int is_equal(int *coord1, int *coord2, int k);

void print_knn(int *target_coord, point_t arr[], int nr_pts, int k);

void store_rs(int *arr, int *elem, int *coord, rs_t *search_area, int k);

void print_coord(int *coord, int k);

#endif  // FUNCTIONS_H
