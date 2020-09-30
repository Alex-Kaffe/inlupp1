#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define int_elem(x)   (elem_t) { .integer = (x) }
#define uint_elem(x)  (elem_t) { .unsigned_int = (x) }
#define bool_elem(x)  (elem_t) { .bool = (x) }
#define float_elem(x) (elem_t) { .floating = (x) }
#define ptr_elem(x)   (elem_t) { .extra = (x) }

#define HAS_ERROR() (errno == EINVAL)
#define SUCCESS(v)  (errno = 0)
#define FAILURE(v)  (errno = EINVAL)

extern int errno;

typedef struct hash_table ioopm_hash_table_t;

typedef struct iter ioopm_list_iterator_t;

typedef struct list ioopm_list_t; /// Meta: struct definition goes in C file

typedef union elem elem_t;

typedef struct compare_data compare_data_t;

union elem {
  int integer;
  unsigned int unsigned_int;
  unsigned long unsigned_long;
  bool boolean;
  float floating;
  void *extra;
};


typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);

/// @brief Used as the extra argument in predicates when comparing values or keys in the hash table
/// This is needed since the value and key are generic types and they are compared using either
/// the hash_func or eq_func. The predicate and apply functions only accept one extra argument
/// of an arbitrary type, meaning that we need a struct to pass in more than one value.
struct compare_data {
  ioopm_eq_function eq_func; // The function used in the comparison
  elem_t element;            // The element to compare to
};

typedef unsigned long(*ioopm_hash_function)(elem_t key);

/// @brief Compares the char* pointers of two elem_t
bool eq_elem_string(elem_t a, elem_t b);

/// @brief Compares the integer value of two elem_t
bool eq_elem_int(elem_t a, elem_t b);