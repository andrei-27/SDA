/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#include "mk.h"

/*
Functia adauga toate cuvintele din path-ul dat in trie.
*/
void trie_load(trie_t *trie, char *path)
{
	char buf[MAX_WORD];

	FILE *inptr = fopen(path, "r");
	DIE(!inptr, "file not found");

	int i = 0;
	while (fscanf(inptr, "%s", buf) == 1) {
		trie_insert(trie, buf);
		++i;
	}

	fclose(inptr);
}

/*
Functia face dfs pe toate cuvintele care incep cu prefixul dat si se opreste
cand diferenta este mai mare decat diferenta data, printand pe parcurs cuvintele
care corespund. Folosesc contorul words_found pentru a sti in functia din care
apelez daca am gasit sau nu cuvinte. La fiecare pas fac o copie a cuvantului
actual si ii modific o litera.
*/
void dfs_autocorrect(trie_node_t *node, char *key, char *current_word,
					 int differences, int max_diff, size_t length,
					 int *words_found)
{
	if (differences > max_diff)
		return;

	if (differences <= max_diff && node->word_occurance &&
		length == strlen(key)) {
		printf("%s\n", current_word);
		(*words_found)++;
	}

	if (length < strlen(key)) {
		for (int i = 0; i < ALPHABET_SIZE; i++) {
			char cp[MAX_WORD], diff = differences, len = length;

			if (node->children[i]) {
				strcpy(cp, current_word);
				cp[length] = ALPHABET[i];
				if (key[length] != ALPHABET[i])
					dfs_autocorrect(node->children[i], key, cp, ++diff,
									max_diff, ++len, words_found);
				else
					dfs_autocorrect(node->children[i], key, cp, diff, max_diff,
									++len, words_found);
			}
		}
	}
}

/*
Functia apeleaza dfs_autocorrect.
*/
void trie_autocorrect(trie_t *trie, char *key, int k)
{
	int length = strlen(key);
	char current_word[MAX_WORD];
	strncpy(current_word, key, length);
	current_word[length] = '\0';

	trie_node_t *current_node = trie->root;
	int differences = 0, wf = 0;
	dfs_autocorrect(current_node, key, current_word, differences, k, 0, &wf);

	if (differences <= k && current_node->word_occurance)
		printf("%s\n", current_word);
	else if (!wf)
		printf("No words found\n");
}

/*
Functia verifica daca avem prefixul dat in trie si merge mereu pe primul copil
pe care il gaseste, pana ajunge la un sfarsit de cuvant si il afiseaza.
*/
void complete_smallest(trie_t *trie, char *key)
{
	trie_node_t *current = trie->root;
	int i = 0, found = 0;
	char word[MAX_WORD];
	strcpy(word, key);

	while (key[i] != '\0') {
		int letter = key[i] - 'a';
		if (current->children[letter]) {
			current = current->children[letter];
		} else {
			printf("No words found\n");
			return;
		}
		++i;
	}

	while (!found) {
		for (int j = 0 ; j < ALPHABET_SIZE ; ++j) {
			if (current->word_occurance) {
				word[i] = '\0';
				found = 1;
				break;
			}
			if (current->children[j]) {
				word[i] = ALPHABET[j];
				current = current->children[j];
				break;
			}
		}
		++i;
	}

	printf("%s\n", word);
}

/*
Functia face dfs pe toate cuvintele care incep cu prefixul dat si salveaza de
fiecare data cuvantul cel mai scurt. La fiecare pas fac o copie a cuvantului
actual si ii modific o litera.
*/
void dfs_autocomplete_shortest(trie_node_t *node, char *word,
							   char *shortest_found, int length)
{
	if (node->word_occurance) {
		word[length] = '\0';
		if (strlen(word) < strlen(shortest_found))
			strcpy(shortest_found, word);
		return;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		char cp[MAX_WORD], len = length;

		if (node->children[i]) {
			strncpy(cp, word, length);
			cp[length] = ALPHABET[i];
			dfs_autocomplete_shortest(node->children[i], cp, shortest_found,
									  ++len);
		}
	}
}

/*
Functia verifica daca avem prefixul dat in trie, apeleaza functia de dfs
corespunzatoare si afiseaza cuvantul gasit.
*/
void complete_shortest(trie_t *trie, char *key)
{
	trie_node_t *current = trie->root;
	int i = 0;
	char word[MAX_WORD], shortest[MAX_WORD];
	strcpy(word, key);

	memset(shortest, ALPHABET[ALPHABET_SIZE - 1], MAX_WORD);
	shortest[MAX_WORD - 1] = '\0';

	while (key[i] != '\0') {
		int letter = key[i] - 'a';
		if (current->children[letter]) {
			current = current->children[letter];
		} else {
			printf("No words found\n");
			return;
		}
		++i;
	}

	dfs_autocomplete_shortest(current, word, shortest, i);
	printf("%s\n", shortest);
}

/*
Functia face dfs pe toate cuvintele care incep cu prefixul dat si salveaza de
fiecare cuvantul cu frecventa cea mai mare si frecventa acestuia. La fiecare pas
fac o copie a cuvantului actual si ii modific o litera.
*/
void dfs_autocomplete_freq(trie_node_t *node, char *word, char *most_freq,
						   int *max_freq, int length)
{
	if (node->word_occurance) {
		word[length] = '\0';
		if (node->word_occurance > (*max_freq)) {
			strcpy(most_freq, word);
			(*max_freq) = node->word_occurance;
		}
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		char cp[MAX_WORD], len = length;

		if (node->children[i]) {
			strncpy(cp, word, length);
			cp[length] = ALPHABET[i];
			dfs_autocomplete_freq(node->children[i], cp, most_freq, max_freq,
								  ++len);
		}
	}
}

/*
Functia verifica daca avem prefixul dat in trie, apeleaza functia de dfs
corespunzatoare si afiseaza cuvantul gasit.
*/
void complete_freq(trie_t *trie, char *key)
{
	trie_node_t *current = trie->root;
	int i = 0, max_freq = 0;
	char word[MAX_WORD], most_freq[MAX_WORD];
	strcpy(word, key);

	memset(most_freq, ALPHABET[ALPHABET_SIZE - 1], MAX_WORD);
	most_freq[MAX_WORD - 1] = '\0';

	while (key[i] != '\0') {
		int letter = key[i] - 'a';
		if (current->children[letter]) {
			current = current->children[letter];
		} else {
			printf("No words found\n");
			return;
		}
		++i;
	}

	dfs_autocomplete_freq(current, word, most_freq, &max_freq, i);
	printf("%s\n", most_freq);
}

/*
Functia apeleaza functiile pentru fiecare din comenzile 0, 1, 2 si 3.
*/
void trie_autocomplete(trie_t *trie, char *key, int nr_crit)
{
	if (nr_crit == 1) {
		complete_smallest(trie, key);
	} else if (nr_crit == 2) {
		complete_shortest(trie, key);
	} else if (nr_crit == 3) {
		complete_freq(trie, key);
	} else {
		complete_smallest(trie, key);
		complete_shortest(trie, key);
		complete_freq(trie, key);
	}
}

/*
In main citim cate o linie si apelam functia corespunzatoare.
*/
int main(void)
{
	int check_sscanf;
	char alphabet[] = ALPHABET;
	char buf[MAX_WORD], key[MAX_WORD], command[MAX_WORD];

	trie_t *trie = trie_create(ALPHABET_SIZE, alphabet);

	fgets(buf, MAX_WORD, stdin);
	check_sscanf = sscanf(buf, "%s", command);

	while (strcmp(command, "EXIT")) {
		if (!strcmp(command, "INSERT")) {
			check_sscanf = sscanf(buf, "%s %s\n", command, key);
			trie_insert(trie, key);
		} else if (!strcmp(command, "REMOVE")) {
			check_sscanf = sscanf(buf, "%s %s\n", command, key);
			trie_remove(trie, key);
		} else if (!strcmp(command, "AUTOCORRECT")) {
			int k_charact;
			check_sscanf = sscanf(buf, "%s %s %d\n", command, key, &k_charact);
			trie_autocorrect(trie, key, k_charact);
		} else if (!strcmp(command, "AUTOCOMPLETE")) {
			int nr_crit;
			check_sscanf = sscanf(buf, "%s %s %d\n", command, key, &nr_crit);
			trie_autocomplete(trie, key, nr_crit);
		} else if (!strcmp(command, "LOAD")) {
			check_sscanf = sscanf(buf, "%s %s\n", command, key);
			trie_load(trie, key);
		}

		fgets(buf, MAX_WORD, stdin);
		check_sscanf = sscanf(buf, "%s", command);
		--check_sscanf; // warning from compiler
	}

	trie_free(&trie);

	return 0;
}
