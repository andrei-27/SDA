# Ⓒ Pirlea Andrei 312CAa 2022-2023

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g -ggdb

# define targets
TARGETS=vma

build: $(TARGETS)

run_vma: build
	./vma

vma: main.c vma.c vma.h list.h list.c functions.c functions.h
	$(CC) $(CFLAGS) *.c -o vma

clean:
	rm -f $(TARGETS)

.PHONY: build clean
