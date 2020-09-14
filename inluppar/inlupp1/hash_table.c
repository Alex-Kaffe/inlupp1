#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "hash_table.h"

// Following the SIMPLE methodology, we are going to dodge and simplify the specification.
// Our first hash table implementation will only support integer keys and string values,
// and only support a fixed number of buckets (17).

// 1. Each index in the array covers a dynamic number of keys7 instead of a single key; and */
// 2. Each value in the array is a sequence of (key,value) pairs so that each entry in the map is represented by a corresponding (key,value) pair (that we will call an entry). */

// ACHIEVEMENT: A3
typedef struct entry entry_t;

// Errors when looking up a key will be saved into the global 'errno' variable
extern int errno;

struct entry
{ 
  int key;       // holds the key
  char *value;   // holds the value
  entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
  entry_t *buckets[17];
};

ioopm_hash_table_t *ioopm_hash_table_create() {
    /// Allocate space for a ioopm_hash_table_t = 17 pointers to
  /// entry_t's, which will be set to NULL
  ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
  return result;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht) {
  free(ht);
}

char *ioopm_hash_table_lookup(ioopm_hash_table_t *ht, int key) {
  entry_t *entry = ht->buckets[key % 17];
  
  if (entry && entry->key == key) {
    
    return entry->value; 
  } else {
    errno = EINVAL;
    return NULL;
  }
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, int key, char *value) {
  
}