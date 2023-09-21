/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#include "functions.h"
#include "kNN.h"

/*
Functia calculeaza distanta Euclidiana la patrat.
*/
int distance_squared(int *coord1, int *coord2, int k)
{
	int dist = 0;
	for (int i = 0 ; i < k ; ++i) {
		int dif = coord1[i] - coord2[i];
		dist += dif * dif;
	}
	return dist;
}

/*
Dintre 2 puncte primite, functia il returneaza pe cel mai apropiat de target.
*/
kd_node_t *closest(int *target_coord, kd_node_t *node1, kd_node_t *node2, int k)
{
	if (!node1)
		return node2;
	if (!node2)
		return node1;
	int dist1 = distance_squared(target_coord, node1->coord, k);
	int dist2 = distance_squared(target_coord, node2->coord, k);
	return (dist1 < dist2) ? node1 : node2;
}

/*
Functia primeste coordonatele unui punct si daca distanta fata de target este
mai mica decat distanta de la primul punct din vectorul solutie, elibereaza
acest vector si il adauga. In cazul in care distantele sunt egale, il adaugam la
vectorul solutie. Iar daca distanta e mai mare, il ignoram.
*/
void store_knn(int *target_coord, int *coord, int k, point_t arr[], int *nr_pts)
{
	int dist = distance_squared(target_coord, coord, k);
	if (*nr_pts) {
		if (distance_squared(arr[0].coord, target_coord, k) > dist) {
			while (*nr_pts)
				free(arr[--(*nr_pts)].coord);

			(*nr_pts) = 0;
			arr[(*nr_pts)].coord = malloc(sizeof(int) * k);
			DIE(!arr[(*nr_pts)].coord, "malloc store_knn");
			memcpy(arr[(*nr_pts)++].coord, coord, k * sizeof(int));
		} else if (distance_squared(arr[0].coord, target_coord, k) == dist) {
			arr[(*nr_pts)].coord = malloc(sizeof(int) * k);
			DIE(!arr[(*nr_pts)].coord, "malloc store_knn");
			memcpy(arr[(*nr_pts)++].coord, coord, k * sizeof(int));
		}
	} else {
		arr[(*nr_pts)].coord = malloc(sizeof(int) * k);
		DIE(!arr[(*nr_pts)].coord, "malloc store_knn");
		memcpy(arr[(*nr_pts)++].coord, coord, k * sizeof(int));
	}
}

/*
Functia verifica daca 2 puncte sunt egale.
*/
int is_equal(int *coord1, int *coord2, int k)
{
	int equal = 1;
	for (int i = 0 ; i < k && equal; ++i)
		if (coord1[i] != coord2[i])
			equal = 0;
	return equal;
}

/*
Functia primeste vectorul de NN, il sorteaza, sterge duplicatele(acestea apar
cand trecem de mai multe ori printr-un nod la parcurgere) si afiseaza vectorul
solutie.
*/
void print_knn(int *target_coord, point_t arr[], int nr_pts, int k)
{
	for (int i = 0 ; i < nr_pts - 1 ; ++i) {
		for (int j = i + 1 ;  j < nr_pts ; ++j) {
			int swap = 0;

			for (int l = 0 ; l < k ; ++l) {
				if (arr[i].coord[l] < arr[j].coord[l])
					break;
				if (arr[i].coord[l] > arr[j].coord[l]) {
					swap = 1;
					break;
				}
			}

			if (swap) {
				int *tmp;
				tmp = arr[i].coord;
				arr[i].coord = arr[j].coord;
				arr[j].coord = tmp;
			}
		}
	}

	for (int i = 0 ; i < nr_pts - 1; ++i) {
		if (is_equal(arr[i].coord, arr[i + 1].coord, k)) {
			free(arr[i].coord);
			for (int j = i ; j < nr_pts - 1 ; ++j)
				arr[j] = arr[j + 1];
			--i;
			--nr_pts;
		}
	}

	for (int i = 0 ; i < nr_pts ; ++i) {
		print_coord(arr[i].coord, k);
		free(arr[i].coord);
	}
}

/*
Functia adauga la vectorul solutie punctele care apartin intervalului.
*/
void store_rs(int *arr, int *elem, int *coord, rs_t *search_area, int k)
{
	int is_valid = 1;

	for (int i = 0 ; i < k ; ++i) {
		if (coord[i] > search_area[i].end || coord[i] < search_area[i].start)
			is_valid = 0;
	}
	if (is_valid) {
		for (int i = 0 ; i < k ; ++i) {
			arr[(*elem)] = coord[i];
			++(*elem);
		}
	}
}

/*
Functia printeaza coordonatele unui punct.
*/
void print_coord(int *coord, int k)
{
	for (int i = 0 ; i < k ; ++i)
		printf("%d ", coord[i]);
	printf("\n");
}
