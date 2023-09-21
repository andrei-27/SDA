/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#include "kdtree.h"
#include "kNN.h"
#include "functions.h"

/*
Functia adauga in kdtree toate punctele din fisier.
*/
kd_node_t *kd_load(char *path, int *k)
{
	kd_node_t *root = NULL;
	int n;

	FILE *inptr = fopen(path, "r");
	DIE(!inptr, "file not found");

	fscanf(inptr, "%d %d\n", &n, k);
	int *coord = malloc(sizeof(int) * (*k));
	DIE(!coord, "malloc kd_load");

	for (int i = 0 ; i < n ; ++i) {
		for (int j = 0 ; j < (*k) ; ++j)
			fscanf(inptr, "%d", &coord[j]);
		root = kd_tree_insert(root, coord, (*k), 0);
	}

	free(coord);
	fclose(inptr);

	return root;
}

/*
Identificam subarborele probabil si alegem la ficare pas dintre radacina si
copilul din subarborele selectat anterior care se afla la o distanta mai mica si
avansam pe el. Daca exista posibilitatea sa avem un punct in celalalt subarbore,
il vizitam si pe acela si apelam functia store_knn cu fiecare "guess" pe care il
avem.
*/
kd_node_t *dfs_knn(kd_node_t *root, int *target_coord, point_t arr[],
				   int *nr_pts, int depth, int k)
{
	if (!root)
		return NULL;

	kd_node_t *next, *maybe;
	if (root->coord[depth % k] > target_coord[depth % k]) {
		next = root->left;
		maybe = root->right;
	} else {
		next = root->right;
		maybe = root->left;
	}

	kd_node_t *tmp = dfs_knn(next, target_coord, arr, nr_pts, depth + 1, k);
	kd_node_t *curr_closest = closest(target_coord, tmp, root, k);

	int bounding_box = target_coord[depth % k] - root->coord[depth % k];
	int radius_sq = distance_squared(target_coord, curr_closest->coord, k);

	if (bounding_box * bounding_box <= radius_sq) {
		tmp = dfs_knn(maybe, target_coord, arr, nr_pts, ++depth, k);
		curr_closest = closest(target_coord, tmp, curr_closest, k);
		store_knn(target_coord, curr_closest->coord, k, arr, nr_pts);
	}

	return curr_closest;
}

/*
Functia citeste coordonatele punctului target, apeleaza dfs_knn pentru aflarea
vecinilor si ii afiseaza prin apelul functiei print_knn.
*/
void nearest_neighbour(kd_node_t *root, char *points, int k)
{
	int *target_coord = malloc(sizeof(int) * k), nr_pts = 0, check_sscanf;
	DIE(!target_coord, "malloc nearest_neighbour");
	point_t arr[100];

	for (int i = 0 ; i < k ; ++i) {
		check_sscanf = sscanf(points, "%d", &target_coord[i]);
		--check_sscanf;
		points = strchr(points, ' ');
		if (points)
			++points;
	}

	dfs_knn(root, target_coord, arr, &nr_pts, 0, k);

	print_knn(target_coord, arr, nr_pts, k);
	free(target_coord);
}

/*
Functia merge recursiv in subarborii care respecta rangeul si apeleaza functia
store_rs pentru fiecare punct pe care il intalneste.
*/
void dfs_rs(kd_node_t *root, rs_t *search_area, int *arr, int *elem, int depth,
			int k)
{
	if (!root)
		return;

	store_rs(arr, elem, root->coord, search_area, k);

	if (root->coord[depth % k] < search_area[depth % k].start) {
		dfs_rs(root->right, search_area, arr, elem, ++depth, k);
	} else if (root->coord[depth % k] > search_area[depth % k].end) {
		dfs_rs(root->left, search_area, arr, elem, ++depth, k);
	} else {
		int depth_copy = depth;
		dfs_rs(root->left, search_area, arr, elem, ++depth_copy, k);
		dfs_rs(root->right, search_area, arr, elem, ++depth, k);
	}
}

/*
Functia citeste intervalul in care vom cauta, apeleaza dfs_rs, iar apoi sorteaza
punctele care respecta intervalul, in final afisandu-le.
*/
void range_search(kd_node_t *root, char *points, int k)
{
	rs_t *search_area = malloc(sizeof(rs_t) * 2 * k);
	DIE(!search_area, "malloc range_search");
	int arr[MAX_WORD], elem = 0, check_sscanf;

	for (int i = 0 ; i < k ; ++i) {
		check_sscanf = sscanf(points, "%d", &search_area[i].start);
		points = strchr(points, ' ');
		++points;
		check_sscanf = sscanf(points, "%d", &search_area[i].end);
		--check_sscanf;
		points = strchr(points, ' ');
		if (points)
			++points;
	}

	dfs_rs(root, search_area, arr, &elem, 0, k);

	for (int i = 0 ; i < elem - k ; i += k) {
		for (int j = i + k ;  j < elem ; j += k) {
			int swap = 0;
			for (int l = 0 ; l < k ; ++l)
				if (arr[i + l] > arr[j + l])
					swap = 1;

			if (swap) {
				for (int l = 0 ; l < k ; ++l) {
					int tmp = arr[i + l];
					arr[i + l] = arr[j + l];
					arr[j + l] = tmp;
				}
			}
		}
	}

	for (int i = 0 ; i < elem ; i++) {
		printf("%d ", arr[i]);
		if ((i + 1) % k == 0)
			printf("\n");
	}

	free(search_area);
}

/*
In main citim cate o linie si apelam functia corespunzatoare.
*/
int main(void)
{
	int k, check_sscanf;
	char buf[MAX_WORD], key[MAX_WORD], command[MAX_WORD];

	kd_node_t *root = NULL;

	fgets(buf, MAX_WORD, stdin);
	check_sscanf = sscanf(buf, "%s", command);

	while (strcmp(command, "EXIT")) {
		if (!strcmp(command, "LOAD")) {
			check_sscanf = sscanf(buf, "%s %s\n", command, key);
			root = kd_load(key, &k);
		} else if (!strcmp(command, "NN")) {
			nearest_neighbour(root, buf + 3, k);
		} else if (!strcmp(command, "RS")) {
			range_search(root, buf + 3, k);
		}

		fgets(buf, MAX_WORD, stdin);
		check_sscanf = sscanf(buf, "%s", command);
		--check_sscanf;
	}

	kd_free(root);

	return 0;
}
