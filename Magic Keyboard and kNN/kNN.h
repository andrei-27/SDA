/* Copyright 2023 <Ⓒ Pirlea Andrei 312CAa 2022-2023> */

#ifndef KNN_H
#define KNN_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define MAX_WORD 256

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);					\
		}							\
	} while (0)

typedef struct rs_t rs_t;
struct rs_t {
	int start;
	int end;
};

typedef struct point_t point_t;
struct point_t {
	int *coord;
};

#endif  // KNN_H
