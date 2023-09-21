# Ⓒ Pirlea Andrei 312CAa 2022-2023

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -Wshadow -Wpedantic -std=c99 -O0 -g

# define targets
TARGETS=kNN mk

build: $(TARGETS)

mk: mk.c mk.h trie.c trie.h
	$(CC) $(CFLAGS) mk.c trie.c -o mk

kNN: kNN.c kNN.h kdtree.c kdtree.h functions.c functions.h
	$(CC) $(CFLAGS) kNN.c kdtree.c functions.c -o kNN

pack:
	zip -FSr 312CA_PirleaAndrei_Tema3.zip README.md Makefile *.c *.h

clean:
	rm -f $(TARGETS) *.o

.PHONY: pack clean
