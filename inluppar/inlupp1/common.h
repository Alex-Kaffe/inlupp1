#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#define int_elem(x)   (elem_t) { .integer = (x) }
#define uint_elem(x)  (elem_t) { .unsigned_int = (x) }
#define bool_elem(x)  (elem_t) { .bool = (x) }
#define float_elem(x) (elem_t) { .floating = (x) }
#define ptr_elem(x)   (elem_t) { .extra = (x) }

#define HAS_ERROR() (errno == EINVAL)
#define SUCCESS(v)  (errno = 0)
#define FAILURE(v)  (errno = EINVAL)

//TODO: Ingen läkning gjordes
extern int errno;

typedef struct hash_table ioopm_hash_table_t;

typedef struct iter ioopm_list_iterator_t;

typedef struct list ioopm_list_t; /// Meta: struct definition goes in C file

typedef union elem elem_t;

union elem {
  int integer;
  unsigned int unsigned_int;
  bool boolean;
  float floating;
  void *extra;
};

typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);

// TODO: Should the hash function return an unsigned int?
typedef unsigned int(*ioopm_hash_function)(elem_t key);
