#define  _POSIX_C_SOURCE 200809L
#include "vma.h"
#include "functions.h"
#include "list.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Functia citeste fiecare linie, iar apoi in functie de actiunea necesara citeste
parametrii necesari din stringul ce contine linia respectiva. In cazul mprotect
sunt parcurse permisiunile, facandu-se sau logic cu fiecare in parte pentru a
obtine permisiunea finala. In cazul instructiunii write este alocat un pointer
ce va contine toate cele "size" caractere aflate dupa valoarea "size", acesta
fiind pasat functiei de write.
*/
void read_data(void)
{
	arena_t *arena;
	char *action = malloc(NMAX * sizeof(char));
	DIE(!action, "malloc failed");

	do {
		char *str = malloc(NMAX);
		uint64_t size = NMAX, adr;
		getline(&str, &size, stdin);

		if (strlen(str) > 1) {
			if (sscanf(str, "%s", action) == 0)
				continue;
		} else {
			free(str);
			continue;
		}

		if (!strcmp(action, "ALLOC_ARENA")) {
			if (sscanf(str, "%s%lu", action, &size) != 0)
				arena = alloc_arena(size);
		} else if (!strcmp(action, "ALLOC_BLOCK")) {
			if (sscanf(str, "%s%lu%lu", action, &adr, &size) != 0)
				alloc_block(arena, adr, size);
		} else if (!strcmp(action, "FREE_BLOCK")) {
			if (sscanf(str, "%s%lu", action, &adr) != 0)
				free_block(arena, adr);
		} else if (!strcmp(action, "PMAP")) {
			pmap(arena);
		} else if (!strcmp(action, "MPROTECT")) {
			int8_t perm = 0b000;
			char *token, delim[4] = " |\n";
			if (sscanf(str, "%s%lu", action, &adr) != 0)
				token = strtok(str, delim);
			while (token) {
				if (!strcmp(token, "PROT_NONE"))
					perm = perm | 0b000;
				if (!strcmp(token, "PROT_READ"))
					perm = perm | 0b100;
				if (!strcmp(token, "PROT_WRITE"))
					perm = perm | 0b010;
				if (!strcmp(token, "PROT_EXEC"))
					perm = perm | 0b001;
				token = strtok(NULL, delim);
			}
			mprotect(arena, adr, perm);
		} else if (!strcmp(action, "WRITE")) {
			char *buff = calloc(NMAX, 1);
			if (sscanf(str, "%s%lu%lu%s", action, &adr, &size, buff) != 0) {
				char *p = strstr(str + strlen(action), buff);
				if (strlen(p) == strlen(str + strlen(action)))
					strcpy(p, "\n");
				write(arena, adr, size, (int8_t *)p);
			}
			free(buff);
		} else if (!strcmp(action, "READ")) {
			if (sscanf(str, "%s%lu%lu", action, &adr, &size) != 0)
				read(arena, adr, size);
		} else if (strcmp(action, "DEALLOC_ARENA")) {
			char *token, delim[3] = "\n ";
			token = strtok(str, delim);
			while (token) {
				printf("Invalid command. Please try again.\n");
				token = strtok(NULL, delim);
			}
		}
		refresh_block_size(arena);
		free(str);

	} while (strcmp(action, "DEALLOC_ARENA"));

	free(action);
	dealloc_arena(arena);
}

int main(void)
{
	read_data();
	return 0;
}
