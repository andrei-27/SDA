#include "list.h"

list_t *dll_create(void)
{
	list_t *list = (list_t *)malloc(sizeof(*list));

	list->size = 0;
	list->head = NULL;

	return list;
}

/*
 * Functia intoarce un pointer la nodul de pe pozitia n din lista.
 * Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se
 * afla pe pozitia n=0). Daca n >= nr_noduri, atunci se intoarce nodul de pe
 * pozitia rezultata daca am "cicla" (posibil de mai multe ori) pe lista si am
 * trece de la ultimul nod, inapoi la primul si am continua de acolo. Cum putem
 * afla pozitia dorita fara sa simulam intreaga parcurgere?
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t *dll_get_nth_node(list_t *list, unsigned int n)
{
	dll_node_t *node = NULL;
	if (!list || !list->head)
		return node;

	node = list->head;
	n %= (list->size + 1);
	while (n--)
		node = node->next;

	return node;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Cand indexam pozitiile nu "ciclam" pe lista circulara ca la
 * get, ci consideram nodurile in ordinea de la head la ultimul (adica acel nod
 * care pointeaza la head ca nod urmator in lista). Daca n >= nr_noduri, atunci
 * adaugam nodul nou la finalul listei.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
void dll_add_nth_node(list_t *list, unsigned int n)
{
	if (!list)
		return;

	if (n > list->size)
		n = list->size;

	dll_node_t *curr = dll_get_nth_node(list, n);

	dll_node_t *node = malloc(sizeof(*node));

	if (list->size == 0) {
		list->head = node;
		node->next = node;
		node->prev = node;
	} else {
		node->prev = curr->prev;
		node->next = curr;
		curr->prev->next = node;
		curr->prev = node;

		if (n == 0)
			list->head = node;
	}

	list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Functia intoarce un pointer spre acest nod
 * proaspat eliminat din lista. Daca n >= nr_noduri - 1, se elimina nodul de la
 * finalul listei. Este responsabilitatea apelantului sa elibereze memoria
 * acestui nod.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t *dll_remove_nth_node(list_t *list, unsigned int n)
{
	if (!list || !list->head)
		return NULL;

	if (n >= list->size)
		n = list->size - 1;
	dll_node_t *curr = dll_get_nth_node(list, n);

	if (list->size == 1) {
		list->head = NULL;
	} else if (curr == list->head) {
		curr->prev->next = curr->next;
		curr->next->prev = curr->prev;
		list->head = curr->next;
	} else {
		curr->prev->next = curr->next;
		curr->next->prev = curr->prev;
	}

	list->size--;

	return curr;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista.
 */
void dll_free(list_t **list)
{
	if (!(*list) || !(*list)->head)
		return;

	dll_node_t *curr = (*list)->head, *del;

	while ((*list)->size--) {
		del = curr;
		curr = curr->next;
		free(del->data);
		free(del);
	}
	free((*list));
}
