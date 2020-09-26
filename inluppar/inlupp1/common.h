#pragma once

#include <stdlib.h>
#include <stdbool.h>

#define int_elem(x) (ioopm_elem_t) { .i = (x) }
#define ptr_elem(x) (ioopm_elem_t) { .p = (x) }

#define HAS_ERROR() (errno == EINVAL)
#define SUCCESS(v)  (errno = 0)
#define FAILURE(v)  (errno = EINVAL)

//TODO: Ingen läkning gjordes.

typedef struct hash_table ioopm_hash_table_t;

typedef struct iter ioopm_list_iterator_t;

typedef struct list ioopm_list_t; /// Meta: struct definition goes in C file


typedef struct elem elem_t;


//
struct elem {
  int _int;
  unsigned int unsigned_int;
  bool b;
  float f;
  void *p;
};