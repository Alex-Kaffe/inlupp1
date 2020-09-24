#pragma once

#include <stdlib.h>
#include <stdbool.h>


typedef struct hash_table ioopm_hash_table_t;

typedef struct iter ioopm_list_iterator_t;

typedef struct list ioopm_list_t; /// Meta: struct definition goes in C file


typedef struct elem elem_t;

struct elem {
  int _int;
  unsigned int unsigned_int;
  bool b;
  float f;
  void *p;
};