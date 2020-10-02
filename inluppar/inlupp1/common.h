#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

//Macros to easier insert elements of different types into the list or table.
#define int_elem(x)   (elem_t) { .integer = (x) }
#define uint_elem(x)  (elem_t) { .unsigned_int = (x) }
#define ulong_elem(x) (elem_t) { .unsigned_long = (x) }
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

/// @brief Union type for storing generic data 
typedef union elem elem_t;

/// @brief Used as the extra argument in predicates when comparing values or keys in the hash table
/// This is needed since the value and key are generic types and they are compared using either
/// the hash_func or eq_func. The predicate and apply functions only accept one extra argument
/// of an arbitrary type, meaning that we need a struct to pass in more than one value.
typedef struct compare_data compare_data_t;

typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);

typedef unsigned long(*ioopm_hash_function)(elem_t key);

union elem {
  int integer;
  unsigned int unsigned_int;
  unsigned long unsigned_long;
  bool boolean;
  float floating;
  void *extra;
};

struct compare_data {
  ioopm_eq_function eq_func; // The function used in the comparison
  elem_t element;            // The element to compare to
};

/// @brief Compares the char* pointers of two elem_t
/// Supports both empty and non-empty strings. 
/// If either a or b is NULL, only the pointers are compared.
/// @param a an element containing a pointer to a NULL terminated string
/// @param b an element containing a pointer to a NULL terminated string
bool eq_elem_string(elem_t a, elem_t b);

/// @brief Compares the integer value of two elem_t
/// @param a an element containing an integer value
/// @param b an element containing an integer value
bool eq_elem_int(elem_t a, elem_t b);

/// @brief Compute a polynomial with the coefficients of the ASCII-values for each individual character
/// @param key a key containing a pointer to a NULL terminated string
/// @returns a hash code based on the NULL terminated string pointed to by key
unsigned long string_knr_hash(elem_t key);