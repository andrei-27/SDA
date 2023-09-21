# Ⓒ Pirlea Andrei 312CAa 2022-2023

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g

# define targets
TARGETS=main

build: $(TARGETS)

main: main.c hashtable.h hashtable.c server.h server.c load_balancer.h load_balancer.c utils.h
	$(CC) $(CFLAGS) *.c -lm -o tema2

clean:
	rm -f $(TARGETS)

.PHONY: build clean