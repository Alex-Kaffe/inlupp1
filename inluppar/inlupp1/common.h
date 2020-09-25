#pragma once

#include <stdlib.h>
#include <stdbool.h>

#define int_elem(x) (elem_t) { .i = (x) }
#define ptr_elem(x) (elem_t) { .p = (x) }

#define HAS_ERROR() (errno == EINVAL)
#define SUCCESS(v)  (errno = 0)
#define FAILURE(v)  (errno = EINVAL)

//TODO: Ingen läkning gjordes.

typedef struct hash_table ioopm_hash_table_t;

typedef struct iter ioopm_list_iterator_t;

typedef struct list ioopm_list_t; /// Meta: struct definition goes in C file

typedef struct elem elem_t;

struct elem {
  int i;
  unsigned int unsigned_int;
  bool b;
  float f;
  void *p;
};

/*
bool eq_elem_int(elem_t a, elem_t b){
  return a.i == b.i;
}

bool eq_elem_string(elem_t a, elem_t b){
  void *p1 = a.p;
  void *p2 = b.p;
  
  return strcmp((char *)p1, (char *)p2) == 0;
}*/
